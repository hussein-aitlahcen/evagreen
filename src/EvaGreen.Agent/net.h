#include <stdint.h>
#include <time.h>

typedef enum e_cmsg_opcode {
    CMSG_CONFIGREQ = 0x0A,
    CMSG_UPLOADDATA = 0x0B
} OpCode;

typedef enum e_data_type {
    DATA_IMAGE = 0x01,
    DATA_TEMPERATURE = 0x02
} DataType;

#pragma pack(push, 1)
typedef struct network_header_t
{
    int32_t data_size;
    uint8_t op_code;
    int32_t agent_id;
} network_header;

typedef struct agent_remote_conf_t
{
    time_t upload_interval;
    time_t snapshot_interval;
    int32_t resolution_w;
    int32_t resolution_h;
    uint8_t initial_contact;
} agent_remote_conf;
#pragma pack(pop)