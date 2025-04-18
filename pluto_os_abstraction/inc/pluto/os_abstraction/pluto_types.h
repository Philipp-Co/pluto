#ifndef __PLUTO_TYPES_H__
#define __PLUTO_TYPES_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <stdint.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

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
    struct timeval timestamp;
    size_t max_bytes_payload;
    char *payload;
    uint32_t id;
    uint32_t event;
};

typedef struct PLUTO_Request* PLUTO_Request_t;

struct PLUTO_Response
{
    uint32_t id;
    uint32_t event;
    struct timeval timestamp;
    char *body;
};

typedef struct PLUTO_Response* PLUTO_Response_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Create a PLUTO_Key_t.
/// \param[in] path - Path to a Directory.
/// \param[in] name - Name of a File that is assigned to the Key.
/// \param[out] key - Output Object. This Function assigned values to this Object. 
/// \return bool - true on success and false otherwise.
///
bool PLUTO_CreateKey(const char *path, const char *name, PLUTO_Key_t *key);
bool PLUTO_KeyGet(const char *path, const char *name, PLUTO_Key_t *key);
///
/// \brief  Destroy a given Key.
///
void PLUTO_DestroyKey(PLUTO_Key_t *key);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
