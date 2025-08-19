///
/// \brief System V Implementation of Semaphore Interface.
///
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/pluto_semaphore.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_Semaphore_t PLUTO_CreateSemaphore(const char *path, const char *name, PLUTO_Logger_t logger)
{
    PLUTO_Semaphore_t semaphore = PLUTO_Malloc(sizeof(struct PLUTO_Semaphore));
    semaphore->logger = logger; 

    if(!PLUTO_CreateKey(path, name, &semaphore->key))
    {
        PLUTO_LoggerWarning(logger, "Unable to create key for %s%s: %s", path, name, strerror(errno));
        goto error;
    }
    int flags = IPC_CREAT;
    int permission = 0777;
    semaphore->filedescriptor = semget(semaphore->key.key, 1, flags | IPC_EXCL | permission);   
    if(semaphore->filedescriptor < 0)
    {
        PLUTO_LoggerInfo(logger, "Semaphore %s%s already exists", path, name);
        semaphore->filedescriptor = semget(semaphore->key.key, 1, flags | permission);   
        if(semaphore->filedescriptor < 0)
        {
            PLUTO_LoggerWarning(logger, "Unable to open Semaphore: %s", strerror(errno));
            goto error;
        }
        return semaphore;
    }
    semctl(semaphore->filedescriptor, 0, SETALL, 0);
    return semaphore;
error:
    PLUTO_DestroySemaphore(&semaphore);
    return NULL;
} 

PLUTO_Semaphore_t PLUTO_SemaphoreGet(const char *path, const char *name, PLUTO_Logger_t logger)
{
    PLUTO_Semaphore_t semaphore = PLUTO_Malloc(sizeof(struct PLUTO_Semaphore));
    semaphore->logger = logger; 
    if(!PLUTO_KeyGet(path, name, &semaphore->key))
    {
        PLUTO_LoggerWarning(logger, "Unable to Create Key for %s%s", path, name);
        goto error;
    }
    semaphore->filedescriptor = semget(semaphore->key.key, 1, 0);
    if(semaphore->filedescriptor < 0)
    {
        PLUTO_LoggerWarning(logger, "Unable to create Semaphore for key_t 0x%x", semaphore->key.key);
        goto error;
    }
    return semaphore;
error:
    PLUTO_LoggerWarning(logger, "Error Semaphore: %s", strerror(errno));
    PLUTO_DestroySemaphore(&semaphore);
    return NULL;
}

void PLUTO_DestroySemaphore(PLUTO_Semaphore_t *semaphore)
{
    if(*semaphore)
    {
        if(PLUTO_SemaphoreValue(*semaphore) <= 0)
        {
            semctl(
                (*semaphore)->filedescriptor, 0, IPC_RMID
            );
        }
        PLUTO_DestroyKey(&(*semaphore)->key);
        PLUTO_Free(*semaphore);
    }
} 

PLUTO_SEM_ReturnValue_t PLUTO_SemaphoreWait(PLUTO_Semaphore_t semaphore)
{
    struct sembuf sops;
    sops.sem_num = 0U;                      // Operate in sem 0.
    sops.sem_op = -1;                       // -1 to semval.
    sops.sem_flg = SEM_UNDO | IPC_NOWAIT;   // 

    const int result = semop(semaphore->filedescriptor, &sops, 1);
    if(result < 0)
    {
        if(EAGAIN == errno)
        {
            return PLUTO_SEM_EAGAIN;
        }
        return PLUTO_SEM_ERROR;   
    }
    return PLUTO_SEM_OK;
} 

PLUTO_SEM_ReturnValue_t PLUTO_SemaphoreSignal(PLUTO_Semaphore_t semaphore)
{
    struct sembuf sops;
    sops.sem_num = 0U;              // Operate in sem 0.
    sops.sem_op = 1U;               // +1 to semval.
    sops.sem_flg = 0U | IPC_NOWAIT; // 
    
    const int result = semop(semaphore->filedescriptor, &sops, 1);
    if(result < 0)
    {
        if(EAGAIN == errno)
        {
            return PLUTO_SEM_EAGAIN;
        }
        return PLUTO_SEM_ERROR;   
    }
    return PLUTO_SEM_OK;
} 

int32_t PLUTO_SemaphoreValue(PLUTO_Semaphore_t semaphore)
{
    return semctl(semaphore->filedescriptor, 0, GETVAL);
} 

//
// --------------------------------------------------------------------------------------------------------------------
//
