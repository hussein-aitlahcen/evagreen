#include <stdint.h>
#include <time.h>

typedef enum e_cmsg_opcode {
    CMSG_CONFIGREQ = 0x0A,
    CMSG_UPLOADDATA = 0x0B
} op_code;

typedef enum e_data_type {
    DATA_IMAGE = 0x01,
    DATA_MEASURE = 0x02
} data_type;

typedef enum e_measure_type {
    MEASURE_TEMPERATURE = 0x01,
    MEASURE_HUMIDITY = 0x02
} measure_type;

#pragma pack(push, 1)
typedef struct network_header_t
{
    int32_t data_size;
    uint8_t op_code;
    int32_t agent_id;
} network_header;

typedef struct agent_remote_conf_t
{
    int64_t upload_interval;
    int64_t snapshot_interval;
    int32_t resolution_w;
    int32_t resolution_h;
    int8_t initial_contact;
} agent_remote_conf;
#pragma pack(pop)