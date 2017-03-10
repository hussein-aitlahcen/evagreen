#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <stdarg.h>
#include <dirent.h>

#define IMAGE_DIRECTORY "./images/"
#define IMAGE_EXTENSION ".png"
#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 1337

#define CONF_REMOTE_FILE "./remote.conf.bin"
#define CONF_LOCAL_FILE "./local.conf.bin"
#define PACKAGE_DIR "./package"

#define DEF_READ_TYPE(type, file)                                   \
    void read_##type(type *ptr)                                     \
    {                                                               \
        FILE *rfptr = NULL;                                         \
        if ((rfptr = fopen(file, "rb")) != NULL)                    \
        {                                                           \
            if (fread(ptr, 1, sizeof(type), rfptr) != sizeof(type)) \
            {                                                       \
                if (!feof(rfptr))                                   \
                {                                                   \
                    fprintf(stderr, "error: reading file\n");       \
                    fclose(rfptr);                                  \
                    exit(1);                                        \
                }                                                   \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            fprintf(stderr, "error: opening file for reading\n");   \
            exit(1);                                                \
        }                                                           \
        fclose(rfptr);                                              \
        exit(1);                                                    \
    }

#define DEF_WRITE_TYPE(type, file)                                   \
    void save_##type(type *ptr)                                      \
    {                                                                \
        FILE *wfptr = NULL;                                          \
        if ((wfptr = fopen(file, "wb")) != NULL)                     \
        {                                                            \
            if (fwrite(ptr, 1, sizeof(type), wfptr) != sizeof(type)) \
            {                                                        \
                fprintf(stderr, "error: saving file\n");             \
                fclose(wfptr);                                       \
                exit(1);                                             \
            }                                                        \
        }                                                            \
        else                                                         \
        {                                                            \
            fprintf(stderr, "error: opening file for writing\n");    \
            exit(1);                                                 \
        }                                                            \
        fclose(wfptr);                                               \
        exit(1);                                                     \
    }

#define DEF_SERIALIZER(type, file) \
    DEF_READ_TYPE(type, file)      \
    DEF_WRITE_TYPE(type, file)

typedef enum e_cmsg_opcode {
    CMSG_CONFIGREQ = 0x0A,
    CMSG_UPLOADDATA = 0x0B
} OpCode;

typedef enum e_data_type {
    DATA_IMAGE = 0x01,
    DATA_TEMPERATURE = 0x02
} DataType;

typedef struct agent_remote_conf_t
{
    time_t upload_interval;
    time_t snapshot_interval;
    uint32_t resolution_w;
    uint32_t resolution_h;
} agent_remote_conf;

typedef struct agent_local_conf_t
{
    time_t last_upload;
    time_t last_snapshot;
} agent_local_conf;

#pragma pack(push, 1)
typedef struct network_header_t
{
    uint32_t size;
    uint8_t op;
} network_header;
#pragma pack(pop)

typedef struct file_content_t
{
    char *path;
    int64_t last_modified;
    uint32_t size;
    int8_t *payload;
} file_content;

DEF_SERIALIZER(agent_remote_conf, CONF_REMOTE_FILE)
DEF_SERIALIZER(agent_local_conf, CONF_LOCAL_FILE)

void log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    FILE *f = fopen("log.txt", "a+");
    if (f != NULL)
    {
        vfprintf(f, format, args);
    }
    vprintf(format, args);
    va_end(args);
}

int str_ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

file_content *read_file(char *path)
{
    file_content *fcontent = (file_content *)malloc(sizeof(file_content));
    fcontent->path = path;
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    fcontent->size = ftell(f);
    fseek(f, 0, SEEK_SET);
    fcontent->payload = (int8_t *)malloc(fcontent->size);
    fread(fcontent->payload, fcontent->size, 1, f);
    fclose(f);

    struct stat attr;
    stat(path, &attr);
    fcontent->last_modified = attr.st_mtime;

    return fcontent;
}

agent_local_conf *load_local_conf()
{
    agent_local_conf *local = (agent_local_conf *)malloc(sizeof(agent_local_conf));
    read_agent_local_conf(local);
    return local;
}

agent_remote_conf *load_remote_conf()
{
    agent_remote_conf *remote = (agent_remote_conf *)malloc(sizeof(agent_remote_conf));
    read_agent_remote_conf(remote);
    return remote;
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

int network_initialized = 0;
void init_network()
{
    if (network_initialized)
        return;

    log("initializing network\n");

    network_initialized = 1;
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
    recv(socket, dst, dst_length, 0);
}

void send_data(int32_t socket, OpCode opcode, uint8_t *payload, uint32_t payload_length)
{
    uint32_t total_size = sizeof(network_header) + payload_length;
    network_header header = {.size = payload_length, .op = opcode};
    uint8_t buffer[total_size];
    memcpy(buffer, &header, sizeof(network_header));
    if (payload != NULL)
    {
        memcpy(buffer + sizeof(network_header), payload, payload_length);
    }
    send(socket, buffer, total_size, 0);
}

void send_data_object(int32_t socket, DataType type, uint8_t *data, uint32_t data_length)
{
    uint32_t total_length = sizeof(uint8_t) + data_length;
    uint8_t buffer[total_length];
    buffer[0] = type;
    memcpy(&buffer[1], data, data_length);
    send_data(socket, CMSG_UPLOADDATA, buffer, total_length);
}

void send_data_temperature(int32_t socket, int32_t temperature)
{
    uint8_t data[4];
    *((int32_t *)&data) = temperature;
    send_data_object(socket, DATA_TEMPERATURE, data, sizeof(int32_t));
}

void send_data_image(int32_t socket, file_content *image)
{
    /*
        data buffer = [file_name_length       file_name       payload_length      payload]
    */
    char *file_name = basename(image->path);
    int32_t file_name_length = strlen(file_name);
    int32_t data_length = sizeof(int32_t) + file_name_length + sizeof(int32_t) + image->size + sizeof(int64_t);
    uint8_t data[data_length];
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
                file_content *image_content = read_file(image_path);
                printf("name=%s, size=%d\n", image_path, image_content->size);
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
    log("initializing local configuration\n");
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
    log("initializing remote configuration\n");
    send_data(socket, CMSG_CONFIGREQ, NULL, 0);
    agent_remote_conf *remote = (agent_remote_conf *)malloc(sizeof(agent_remote_conf));
    read_data(socket, remote, sizeof(agent_remote_conf));
    log("remote conf received.\n");
    log("upload_interval=%d\n", remote->upload_interval);
    log("snapshot_interval=%d\n", remote->snapshot_interval);
    log("resolution_w=%d\n", remote->resolution_w);
    log("resolution_h=%d\n", remote->resolution_h);
    save_agent_remote_conf(remote);
    free(remote);
}

/*
    First step, check whenever we are booting the app for the first time.
    If so, the config should be loaded and a first image should be taken
*/
int32_t bootstrap_config()
{
    init_local_conf();
    int32_t socket = connect_socket();

    init_remote_conf(socket);

    send_images(socket);

    close(socket);
}

/*
    1 : Boot config
    2 : Check if an update is required
        If so, boot up the 3G key and upload the package + update the remote conf
*/
int32_t main(int argc, char **argv)
{
    bootstrap_config();
    return 0;
};
