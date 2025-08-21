
#include <pluto/os_abstraction/config/pluto_compile_config.h>

#if PLUTO_OS_INTERFACE == (PLUTO_OS_INTERFACE_SYSTEM_V)

#include <pluto/os_abstraction/pluto_shared_memory.h>
#include <pluto/os_abstraction/pluto_malloc.h>
#include <pluto/os_abstraction/pluto_types.h>

#include <assert.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


PLUTO_SharedMemory_t PLUTO_CreateSharedMemory(size_t nbytes, const char *path, const char *name, PLUTO_Logger_t logger)
{
    PLUTO_Key_t key = {.file=NULL, .path_to_file=NULL}; 
    //if(!PLUTO_KeyGet(path, name, &key))
    //{
    if(!PLUTO_CreateKey(path, name, &key))
    {
        PLUTO_LoggerWarning(logger, "Unable to create create Shared Memory for %s%s", path, name);
        return NULL;
    }
    //} 
    PLUTO_LoggerInfo(logger, "Open Shm with File %s%s", path, name);
    
    const size_t size = nbytes;
    int shm_fd = shmget(key.key, size, IPC_CREAT | 0777);
    if(shm_fd < 0)
    {
        PLUTO_LoggerWarning(logger, "shmget returned with an Error, errno: %s", strerror(errno));
        PLUTO_DestroyKey(&key);
        return NULL;
    }

    void *address = shmat(shm_fd, NULL, 0);
    if(!address)
    {
        PLUTO_LoggerWarning(logger, "shmat returned with an Error, errno: %s", strerror(errno));
        close(shm_fd);
        PLUTO_DestroyKey(&key);
        return NULL;
    }

    PLUTO_SharedMemory_t shm = (PLUTO_SharedMemory_t)PLUTO_Malloc(sizeof(struct PLUTO_SharedMemory));
    shm->address = address;
    shm->shm_fd = shm_fd;
    PLUTO_DestroyKey(&key);
    return shm;
}

void PLUTO_DestroySharedMemory(PLUTO_SharedMemory_t *shm)
{
    //
    // Check how to delete Shared Memory!
    //
    assert(NULL != shm);
    assert(NULL != *shm);
    PLUTO_Free(*shm);
    *shm = NULL;
}

void* PLUTO_SharedMemoryAddress(PLUTO_SharedMemory_t shm)
{
    return shm->address;
}
#else
#error "PLUTO Shared Memory not implemented!"
#endif

