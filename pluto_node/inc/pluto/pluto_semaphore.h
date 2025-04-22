#ifndef __PLUTO_SEMAPHORE_H__
#define __PLUTO_SEMAPHORE_H__

#include <pluto/pluto_types.h>

#include <stdint.h>


struct PLUTO_Semaphore 
{
    PLUTO_Key_t key;
    int filedescriptor;
};
typedef struct PLUTO_Semaphore* PLUTO_Semaphore_t;

typedef enum
{
    PLUTO_SEM_OK = 0,
    PLUTO_SEM_EAGAIN  = 1,
    PLUTO_SEM_ERROR = 2
} PLUTO_SEM_ReturnValue_t;

PLUTO_Semaphore_t PLUTO_CreateSemaphore(const char *path, const char *name);
void PLUTO_DestroySemaphore(PLUTO_Semaphore_t *semaphore);
PLUTO_SEM_ReturnValue_t PLUTO_SemaphoreWait(PLUTO_Semaphore_t semaphore); 
PLUTO_SEM_ReturnValue_t PLUTO_SemaphoreSignal(PLUTO_Semaphore_t semaphore); 
int32_t PLUTO_SemaphoreValue(PLUTO_Semaphore_t semaphore); 

#endif
