#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define closesocket(s) close(s)

#define CONF_REMOTE_FILE "./remote.conf.bin"
#define CONF_LOCAL_FILE "./local.conf.bin"
#define PACKAGE_DIR "./package"

#define CMSG_CONFIGREQ 0x00
#define CMSG_PACKAGEUP 0x01
#define SMSG_CONFIGREP 0x02

#define DEF_READ_TYPE(type, file)                                   \
    int read_##type(type *ptr)                                      \
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
                    return 0;                                       \
                }                                                   \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            fprintf(stderr, "error: opening file for reading\n");   \
            return 0;                                               \
        }                                                           \
        fclose(rfptr);                                              \
        return 1;                                                   \
    }

#define DEF_WRITE_TYPE(type, file)                                   \
    int save_##type(type *ptr)                                       \
    {                                                                \
        FILE *wfptr = NULL;                                          \
        if ((wfptr = fopen(file, "wb")) != NULL)                     \
        {                                                            \
            if (fwrite(ptr, 1, sizeof(type), wfptr) != sizeof(type)) \
            {                                                        \
                fprintf(stderr, "error: saving file\n");             \
                fclose(wfptr);                                       \
                return 0;                                            \
            }                                                        \
        }                                                            \
        else                                                         \
        {                                                            \
            fprintf(stderr, "error: opening file for writing\n");    \
            return 0;                                                \
        }                                                            \
        fclose(wfptr);                                               \
        return 1;                                                    \
    }

#define DEF_SERIALIZER(type, file) \
    DEF_READ_TYPE(type, file)      \
    DEF_WRITE_TYPE(type, file)

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

DEF_SERIALIZER(agent_remote_conf, CONF_REMOTE_FILE)
DEF_SERIALIZER(agent_local_conf, CONF_LOCAL_FILE)

agent_local_conf *load_local_conf()
{
    agent_local_conf *local = malloc(sizeof(agent_local_conf));
    read_agent_local_conf(local);
    return local;
}

agent_remote_conf *load_remote_conf()
{
    agent_remote_conf *remote = (agent_remote_conf *)malloc(sizeof(agent_remote_conf));
    read_agent_remote_conf(remote);
    return remote;
}

int require_upload(agent_local_conf *local, agent_remote_conf *remote)
{
    time_t current = time(NULL);
    time_t next_upload = local->last_upload + remote->upload_interval;
    if (next_upload < current)
    {
        return 1;
    }
    return 0;
}

int require_snapshot(agent_local_conf *local, agent_remote_conf *remote)
{
    time_t current = time(NULL);
    time_t next_snapshot = local->last_snapshot + remote->snapshot_interval;
    if (next_snapshot < current)
    {
        return 1;
    }
    return 0;
}

/*
    First step, check whenever we are booting the app for the first time
    If so, the config should be loaded and a first image should be taken
*/
int bootstrap_config()
{
}

/*
    1 : Boot config
    2 : Check if an update is required
        If so, boot up the 3G key and upload the package + update the remote conf
*/
int main(int argc, char **argv)
{
    bootstrap_config();
    return 0;
}