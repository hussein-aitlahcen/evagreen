#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <libgen.h>
#include <dirent.h>
#include <inttypes.h>
#include <string.h>

#include "net.h"
#include "var.h"
#include "misc.h"
#include "id.h"

typedef struct agent_local_conf_t
{
    int64_t last_upload;
    int64_t last_snapshot;
} agent_local_conf;

DEF_SERIALIZER(agent_remote_conf, CONF_REMOTE_FILE);
DEF_SERIALIZER(agent_local_conf, CONF_LOCAL_FILE);

int network_initialized = 0;
agent_remote_conf *r_conf;
agent_local_conf *l_conf;

void load_local_conf()
{
    l_conf = (agent_local_conf *)malloc(sizeof(agent_local_conf));
    read_agent_local_conf(l_conf);
}

void load_remote_conf()
{
    r_conf = (agent_remote_conf *)malloc(sizeof(agent_remote_conf));
    read_agent_remote_conf(r_conf);
    LOG("upload_interval=%ld\n", r_conf->upload_interval);
    LOG("snapshot_interval=%ld\n", r_conf->snapshot_interval);
    LOG("width=%d\n", r_conf->resolution_w);
    LOG("height=%d\n", r_conf->resolution_h);
    LOG("initial_contact=%d\n", r_conf->initial_contact);
}

void update_last_upload()
{
    l_conf->last_upload = time(NULL);
    save_agent_local_conf(l_conf);
}

void update_last_snapshot()
{
    l_conf->last_snapshot = time(NULL);
    save_agent_local_conf(l_conf);
}

int32_t time_expired(int64_t last_exec, int64_t interval)
{
    int64_t current = time(NULL);
    int64_t next_exec = last_exec + interval;
    if (next_exec < current)
    {
        return 1;
    }
    return 0;
}

int32_t require_upload()
{
    return time_expired(l_conf->last_upload, r_conf->upload_interval);
}

int32_t require_snapshot()
{
    return time_expired(l_conf->last_snapshot, r_conf->snapshot_interval);
}

int32_t file_exists(const char *path)
{
    return access(path, F_OK) != -1;
}

void take_snapshot()
{
    LOG("taking snapshot\n");
    char cmd[256];
    sprintf(cmd, "./agent_script.sh %d %d\n", r_conf->resolution_w, r_conf->resolution_h);
    LOG("%s\n", cmd);
    system(cmd);
    update_last_snapshot();
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

void send_data(int32_t socket, op_code opcode, int8_t *payload, uint32_t payload_length)
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

void send_data_object(int32_t socket, data_type type, int8_t *data, uint32_t data_length)
{
    int32_t total_length = sizeof(uint8_t) + data_length;
    int8_t buffer[total_length];

    // [e_data_type         data]
    buffer[0] = type;
    memcpy(&buffer[1], data, data_length);
    send_data(socket, CMSG_UPLOADDATA, buffer, total_length);
}

void send_data_measure(int32_t socket, measure_type type, int32_t measure)
{
    LOG("sending measure\n");
    int8_t buffer[sizeof(uint8_t) + sizeof(int32_t)];
    // [e_measure_type         measure]
    buffer[0] = type;
    memcpy(&buffer[1], &measure, sizeof(int32_t));
    send_data_object(socket, DATA_MEASURE, buffer, sizeof(buffer));
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
        system("rm ./images/*.jpg");
    }
    else
    {
        perror("failed to open image directory");
    }
}

void init_local_conf()
{
    if (file_exists(CONF_LOCAL_FILE))
    {
        LOG("loading existing local conf\n");
        load_local_conf();
    }
    else
    {
        LOG("initializing local configuration\n");
        l_conf = (agent_local_conf *)malloc(sizeof(agent_local_conf));
        l_conf->last_upload = 0;
        l_conf->last_snapshot = 0;
        save_agent_local_conf(l_conf);
    }
}

/*
    Initialize remote configuration by asked the server to give it
*/
void init_remote_conf()
{
    if (file_exists(CONF_REMOTE_FILE))
    {
        LOG("loading existing remote conf\n");
        load_remote_conf();
    }
    else
    {
        int32_t socket = connect_socket();
        LOG("initializing remote configuration\n");
        send_data(socket, CMSG_CONFIGREQ, NULL, 0);
        r_conf = (agent_remote_conf *)malloc(sizeof(agent_remote_conf));
        read_data(socket, r_conf, sizeof(agent_remote_conf));
        LOG("remote configuration received.\n");
        save_agent_remote_conf(r_conf);
        close_socket(socket);
    }
}

/*
    First step, check whenever we are booting the app for the first time.
    If so, the config should be loaded.
*/
void bootstrap_config()
{
    init_local_conf();
    init_remote_conf();
}

void on_upload()
{
    int32_t socket = connect_socket();
    // fake measures
    send_data_measure(socket, MEASURE_TEMPERATURE, rand() % 5 + 20);
    send_data_measure(socket, MEASURE_HUMIDITY, rand() % 10 + 70);
    send_images(socket);
    close_socket(socket);
    update_last_upload();
}

/*

*/
void process_logic()
{
    LOG("processing logic\n");
    if (require_snapshot())
    {
        take_snapshot();
    }
    if (require_upload())
    {
        on_upload();
    }
}

/*
    1 : Boot config
    2 : Check if an update is required
        If so, boot up the 3G key and upload the package + update the remote conf
*/
int32_t main(int argc, char **argv)
{
    srand(time(NULL));
    bootstrap_config();
    process_logic();
    return 0;
};
