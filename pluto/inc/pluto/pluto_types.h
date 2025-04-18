#ifndef __PLUTO_TYPES_H__
#define __PLUTO_TYPES_H__

#include <stdint.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    key_t key;
    FILE *file;
    char *path_to_file;
} PLUTO_Key_t;

typedef struct
{
    char *name;
    char *value;
} PLUTO_Parameter_t;

struct PLUTO_Request
{
    size_t max_n_header;
    size_t max_n_query_parameter;
    size_t max_size_parameter_name;
    size_t max_size_parameter_value;
    size_t max_bytes_body;
    size_t n_header;
    size_t n_query_parameter;
    PLUTO_Parameter_t *query_parameter;
    PLUTO_Parameter_t *header;
    char *body;
    uint32_t id;
    uint8_t queue;
};

typedef struct PLUTO_Request* PLUTO_Request_t;

struct PLUTO_Response
{
    uint32_t id;
    char *body;
};

typedef struct PLUTO_Response* PLUTO_Response_t;


bool PLUTO_CreateKey(const char *path, const char *name, PLUTO_Key_t *key);
void PLUTO_DestroyKey(PLUTO_Key_t key);

#endif
