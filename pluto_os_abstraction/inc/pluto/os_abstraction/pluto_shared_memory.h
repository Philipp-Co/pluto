#ifndef __PLUTO_SHARED_MEMORY_H__
#define __PLUTO_SHARED_MEMORY_H__

#include <pluto/os_abstraction/pluto_logger.h>

#include <stddef.h>


struct PLUTO_SharedMemory 
{
    void *address;
    int shm_fd;
};

typedef struct PLUTO_SharedMemory* PLUTO_SharedMemory_t;

PLUTO_SharedMemory_t PLUTO_CreateSharedMemory(size_t nbytes, const char *path, const char *name, PLUTO_Logger_t logger);
void PLUTO_DestroySharedMemory(PLUTO_SharedMemory_t *shm);
void* PLUTO_SharedMemoryAddress(PLUTO_SharedMemory_t shm);

#endif
