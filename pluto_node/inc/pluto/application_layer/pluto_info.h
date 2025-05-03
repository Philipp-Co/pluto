#ifndef __PLUTO_INFO_H__
#define __PLUTO_INFO_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/pluto_types.h>

#include <stdint.h>
#include <fcntl.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

struct PLUTO_Info
{
    PLUTO_Key_t key;
    int shm_filedescriptor;
    char *shm;
};
typedef struct PLUTO_Info* PLUTO_Info_t;

typedef struct
{
    const char *name;

    const char *name_of_input_queue;
    uint16_t n_names_of_output_queues;
    char * const * names_of_output_queues;
    
    uint32_t n_header;
    uint32_t n_query_parameter;
} PLUTO_InfoValues_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_Info_t PLUTO_CreateInfo(const char *path, const char *name);
void PLUTO_DestroyInfo(PLUTO_Info_t *info);
void PLUTO_InfoDisplay(const PLUTO_Info_t info, const PLUTO_InfoValues_t *values);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
