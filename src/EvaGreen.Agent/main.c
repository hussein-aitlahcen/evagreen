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

typedef enum {
    CMSG_CONFIGREQ = 0x0A,
    CMSG_UPLOADDATA = 0x0B
} OpCode;

typedef enum {
    DATA_PHOTO = 0x01,
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

DEF_SERIALIZER(agent_remote_conf, CONF_REMOTE_FILE)
DEF_SERIALIZER(agent_local_conf, CONF_LOCAL_FILE)

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

    printf("initializing network\n");

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

void send_data(int32_t socket, OpCode opcode, uint8_t *payload, uint32_t payload_length)
{
    uint32_t total_size = sizeof(network_header) + payload_length;

    printf("total size: %d\n", total_size);

    network_header header = {.size = payload_length, .op = opcode};

    uint8_t buffer[total_size];

    memcpy(buffer, header, sizeof(network_header));

    if (payload != NULL)
    {
        memcpy(buffer + sizeof(network_header), payload, payload_length);
    }

    send(socket, buffer, total_size, 0);
}

void read_data(int32_t socket, void *dst, ssize_t dst_length)
{
    recv(socket, dst, dst_length, 0);
}

void init_local_conf()
{
    if (file_exists(CONF_LOCAL_FILE))
        return;
    printf("initializing local configuration.\n");
    agent_local_conf *local = (agent_local_conf *)malloc(sizeof(agent_local_conf));
    local->last_upload = 0;
    local->last_snapshot = 0;
    save_agent_local_conf(local);
}

void init_remote_conf(int32_t socket)
{
    if (file_exists(CONF_REMOTE_FILE))
        return;
    printf("initializing remote configuration.\n");
    send_data(socket, CMSG_CONFIGREQ, NULL, 0);
    agent_remote_conf *remote = (agent_remote_conf *)malloc(sizeof(agent_remote_conf));
    read_data(socket, remote, sizeof(agent_remote_conf));
    printf("remote conf received.\n");
    printf("upload_interval=%d\n", remote->upload_interval);
    printf("snapshot_interval=%d\n", remote->snapshot_interval);
    printf("resolution_w=%d\n", remote->resolution_w);
    printf("resolution_h=%d\n", remote->resolution_h);
    save_agent_remote_conf(remote);
    free(remote);
}

void send_data_object(int32_t socket, DataType type, uint8_t *data, uint32_t data_length)
{
    uint32_t total_length = data_length + sizeof(uint8_t);
    uint8_t buffer[total_length];
    buffer[0] = type;
    memcpy(&buffer + 1, data, data_length);
    send_data(socket, CMSG_UPLOADDATA, buffer, total_length);
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

    uint8_t data[4];
    *((int32_t *)&data) = 15;
    send_data_object(socket, DATA_TEMPERATURE, data, 4);
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
