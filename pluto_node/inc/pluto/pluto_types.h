#ifndef __PLUTO_TYPES_H__
#define __PLUTO_TYPES_H__

#include <stdint.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>


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
    time_t timestamp;
    size_t max_bytes_payload;
    char *payload;
    int id;
    int event;
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
