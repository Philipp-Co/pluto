
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/pluto_semaphore.h>

#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_Semaphore_t PLUTO_CreateSemaphore(const char *path, const char *name)
{
    (void)name;
    PLUTO_Semaphore_t semaphore = malloc(sizeof(struct PLUTO_Semaphore));
    
    if(!PLUTO_CreateKey(path, name, &semaphore->key))
    {
        return NULL;
    }
    int flags = IPC_CREAT;
    int permission = 0777;
    semaphore->filedescriptor = semget(semaphore->key.key, 1, flags | IPC_EXCL | permission);   
    if(semaphore->filedescriptor < 0)
    {
        semaphore->filedescriptor = semget(semaphore->key.key, 1, flags | permission);   
        printf("Used Sem with count: %i\n", PLUTO_SemaphoreValue(semaphore));
        return semaphore;
    }
    semctl(semaphore->filedescriptor, 0, SETALL, 0);
    printf("Created Sem with count: %i\n", PLUTO_SemaphoreValue(semaphore));
    return semaphore;
} 

void PLUTO_DestroySemaphore(PLUTO_Semaphore_t *semaphore)
{
    printf("Destroy Sem with count: %i\n", PLUTO_SemaphoreValue(*semaphore));
    PLUTO_DestroyKey((*semaphore)->key);
    free(*semaphore);
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
    printf("Sem -1, %i\n", PLUTO_SemaphoreValue(semaphore));
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
    printf("Sem +1, %i\n", PLUTO_SemaphoreValue(semaphore));
    return PLUTO_SEM_OK;
} 

int32_t PLUTO_SemaphoreValue(PLUTO_Semaphore_t semaphore)
{
    return semctl(semaphore->filedescriptor, 0, GETVAL);
} 

//
// --------------------------------------------------------------------------------------------------------------------
//
