#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>

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
                    LOG("error: reading file\n");                   \
                    fclose(rfptr);                                  \
                }                                                   \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            LOG("error: opening file for reading\n");               \
        }                                                           \
        fclose(rfptr);                                              \
    }

#define DEF_WRITE_TYPE(type, file)                                   \
    void save_##type(type *ptr)                                      \
    {                                                                \
        FILE *wfptr = NULL;                                          \
        if ((wfptr = fopen(file, "wb")) != NULL)                     \
        {                                                            \
            if (fwrite(ptr, 1, sizeof(type), wfptr) != sizeof(type)) \
            {                                                        \
                LOG("error: saving file\n");                         \
                fclose(wfptr);                                       \
            }                                                        \
        }                                                            \
        else                                                         \
        {                                                            \
            LOG("error: opening file for writing\n");                \
        }                                                            \
        fclose(wfptr);                                               \
    }

#define LOG(f_, ...)             \
    printf((f_), ##__VA_ARGS__); \
    file_log((f_), ##__VA_ARGS__);

#define DEF_SERIALIZER(type, file) \
    DEF_READ_TYPE(type, file);     \
    DEF_WRITE_TYPE(type, file);

typedef struct file_content_t
{
    char *path;
    int64_t last_modified;
    int32_t size;
    uint8_t *payload;
} file_content;

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

void file_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    FILE *f = fopen("log.txt", "a+");
    if (f != NULL)
    {
        vfprintf(f, format, args);
    }
    va_end(args);
}

file_content *file_read(char *path)
{
    file_content *fcontent = (file_content *)malloc(sizeof(file_content));
    fcontent->path = path;
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    fcontent->size = ftell(f);
    fseek(f, 0, SEEK_SET);
    fcontent->payload = (uint8_t *)malloc(fcontent->size);
    fread(fcontent->payload, fcontent->size, 1, f);
    fclose(f);

    struct stat attr;
    stat(path, &attr);
    fcontent->last_modified = attr.st_mtime;

    return fcontent;
}
