
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/application_layer/pluto_info.h>
#include <pluto/os_abstraction/pluto_types.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_INFO_SHM_NBYTES (4096)
#define PLUTO_INFO_SHM_PERMISSIONS (0744)

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_Info_t PLUTO_CreateInfo(const char *path, const char *name)
{
    PLUTO_Info_t info = PLUTO_Malloc(sizeof(struct PLUTO_Info));
    
    if(!PLUTO_CreateKey(path, name, &info->key))
    {
        PLUTO_Free(info);
        return NULL;
    } 

    info->shm_filedescriptor = shmget(
        info->key.key,
        PLUTO_INFO_SHM_NBYTES,
        IPC_CREAT | PLUTO_INFO_SHM_PERMISSIONS 
    );
    
    info->shm = shmat(info->shm_filedescriptor, NULL, 0);
    return info;
}

void PLUTO_DestroyInfo(PLUTO_Info_t *info)
{
    shmctl((*info)->shm_filedescriptor, IPC_RMID, NULL);
    PLUTO_DestroyKey(&(*info)->key);
}

void PLUTO_InfoDisplay(const PLUTO_Info_t info, const PLUTO_InfoValues_t *values)
{
    char *buffer = PLUTO_Malloc(
        4096 
    );
    memset(buffer, '\0', 4096);
    
    for(size_t i=0;i<values->n_names_of_output_queues;++i)
    {
        strcat(buffer, "\"");
        strcat(buffer, values->names_of_output_queues[i]);
        strcat(buffer, "\"");
        if(i < (values->n_names_of_output_queues - 1))
            strcat(buffer, ",");
    }

    snprintf(
        info->shm, 
        PLUTO_INFO_SHM_NBYTES, 
        "{"
        "\"name\":\"%s\","
        "\"n_header\":%i,"
        "\"n_query_parameter\":%i,"
        "\"n_output_queues\":%i,"
        "\"name_of_input_queue\":\"%s\","
        "\"names_output_queues\":["
        "%s"
        "]"
        "}",
        values->name,
        values->n_header,
        values->n_query_parameter,
        values->n_names_of_output_queues,
        values->name_of_input_queue,
        buffer
    );

    PLUTO_Free(buffer);
}

//
// --------------------------------------------------------------------------------------------------------------------
//
