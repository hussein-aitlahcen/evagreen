#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <libgen.h>
#include <dirent.h>
#include <inttypes.h>

#include "net.h"
#include "var.h"
#include "misc.h"

typedef struct agent_local_conf_t
{
    time_t last_upload;
    time_t last_snapshot;
} agent_local_conf;

DEF_SERIALIZER(agent_remote_conf, CONF_REMOTE_FILE);
DEF_SERIALIZER(agent_local_conf, CONF_LOCAL_FILE);

int network_initialized = 0;
agent_remote_conf *r_conf;
agent_local_conf *l_conf;

void load_local_conf()
{
    agent_local_conf *local = (agent_local_conf *)malloc(sizeof(agent_local_conf));
    read_agent_local_conf(local);
    l_conf = local;
}

void load_remote_conf()
{
    agent_remote_conf *remote = (agent_remote_conf *)malloc(sizeof(agent_remote_conf));
    read_agent_remote_conf(remote);
    r_conf = remote;
}

int32_t time_expired(time_t last_exec, time_t interval)
{
    time_t current = time(NULL);
    time_t next_exec = last_exec + interval;
    if (next_exec < current)
    {
        return 1;
    }
    return 0;
}

int32_t require_upload(agent_local_conf *local, agent_remote_conf *remote)
{
    return time_expired(local->last_upload, remote->upload_interval);
}

int32_t require_snapshot(agent_local_conf *local, agent_remote_conf *remote)
{
    return time_expired(local->last_snapshot, remote->snapshot_interval);
}

int32_t file_exists(const char *path)
{
    return access(path, F_OK) != -1;
}

void init_network()
{
    if (network_initialized)
        return;

    LOG("initializing network\n");

    network_initialized = 1;
}

void close_socket(int32_t socket)
{
    LOG("shutting down network\n");
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

int32_t connect_socket()
{
    init_network();

    int32_t sockfd;
    struct hostent *he;
    struct sockaddr_in addr;

    if ((he = gethostbyname(SERVER_HOST)) == NULL)
    {
        herror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *)&addr,
                sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        exit(1);
    }

    return sockfd;
}

void read_data(int32_t socket, void *dst, ssize_t dst_length)
{
    size_t r = 0;
    do
    {
        r += read(socket, ((int8_t *)dst) + r, dst_length - r);
    } while (r != dst_length);
}

void send_data(int32_t socket, OpCode opcode, int8_t *payload, uint32_t payload_length)
{
    LOG("sending message: OpCode=%d, Size=%d\n", opcode, payload_length);

    // network_header = [i4_size i1_opcode]
    // [network_header  payload]
    int32_t total_size = sizeof(network_header) + payload_length;
    network_header header =
        {
            .data_size = payload_length,
            .op_code = opcode,
            .agent_id = AGENT_ID,
        };
    int8_t buffer[total_size];
    memcpy(buffer, &header, sizeof(network_header));
    if (payload_length > 0)
    {
        memcpy(&buffer[sizeof(network_header)], payload, payload_length);
    }
    size_t sent = 0;
    do
    {
        sent += write(socket, &buffer[sent], total_size - sent);
    } while (sent != total_size);
}

void send_data_object(int32_t socket, DataType type, int8_t *data, uint32_t data_length)
{
    int32_t total_length = sizeof(uint8_t) + data_length;
    int8_t buffer[total_length];

    // [e_data_type         data]
    buffer[0] = type;
    memcpy(&buffer[1], data, data_length);
    send_data(socket, CMSG_UPLOADDATA, buffer, total_length);
}

void send_data_temperature(int32_t socket, int32_t temperature)
{
    LOG("sending temperature\n");
    int8_t data[sizeof(int32_t)];
    memcpy(&data, &temperature, sizeof(int32_t));
    send_data_object(socket, DATA_TEMPERATURE, data, sizeof(data));
}

void send_data_image(int32_t socket, file_content *image)
{
    /*
        data buffer = [file_name_length       file_name       payload_length      payload]
    */
    LOG("sending image\n");
    char *file_name = basename(image->path);
    int32_t file_name_length = strlen(file_name);
    int32_t data_length = sizeof(int32_t) + file_name_length + sizeof(int32_t) + image->size + sizeof(int64_t);
    int8_t data[data_length];
    uint32_t offset = 0;
    memcpy(&data, &file_name_length, sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(&data[offset], file_name, file_name_length);
    offset += file_name_length;
    memcpy(&data[offset], &image->size, sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(&data[offset], image->payload, image->size);
    offset += image->size;
    memcpy(&data[offset], &image->last_modified, sizeof(int64_t));
    send_data_object(socket, DATA_IMAGE, data, data_length);
}

void send_images(int32_t socket)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(IMAGE_DIRECTORY)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (str_ends_with(ent->d_name, IMAGE_EXTENSION))
            {
                char path[256] = IMAGE_DIRECTORY;
                char *image_path = strcat(path, ent->d_name);
                file_content *image_content = file_read(image_path);
                LOG("image name=%s, size=%d\n", image_path, image_content->size);
                send_data_image(socket, image_content);
                free(image_content->payload);
                free(image_content);
            }
        }
        closedir(dir);
    }
    else
    {
        perror("failed to open image directory");
    }
}

void init_local_conf()
{
    if (file_exists(CONF_LOCAL_FILE))
        return;
    LOG("initializing local configuration\n");
    agent_local_conf *local = (agent_local_conf *)malloc(sizeof(agent_local_conf));
    local->last_upload = 0;
    local->last_snapshot = 0;
    save_agent_local_conf(local);
}

/*
    Initialize remote configuration by asked the server to give it
*/
void init_remote_conf(int32_t socket)
{
    if (file_exists(CONF_REMOTE_FILE))
        return;
    LOG("initializing remote configuration\n");
    send_data(socket, CMSG_CONFIGREQ, NULL, 0);
    agent_remote_conf *remote = (agent_remote_conf *)malloc(sizeof(agent_remote_conf));
    read_data(socket, remote, sizeof(agent_remote_conf));
    LOG("remote configuration received.\n");
    save_agent_remote_conf(remote);
    free(remote);
}

/*
    First step, check whenever we are booting the app for the first time.
    If so, the config should be loaded.
*/
void bootstrap_config(int32_t socket)
{
    init_local_conf();
    init_remote_conf(socket);
    load_local_conf();
    load_remote_conf();
}

/*

*/
void process_logic(int32_t socket)
{
    //send_data_temperature(socket, 12);
    //send_data_temperature(socket, 9);
    send_images(socket);
}

/*
    1 : Boot config
    2 : Check if an update is required
        If so, boot up the 3G key and upload the package + update the remote conf
*/
int32_t main(int argc, char **argv)
{
    int32_t socket = connect_socket();
    bootstrap_config(socket);
    process_logic(socket);
    close_socket(socket);
    return 0;
};
