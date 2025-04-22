///
/// \brief  Implementation of a Semaphore.
///
#ifndef __PLUTO_SEMAPHORE_H__
#define __PLUTO_SEMAPHORE_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_types.h>

#include <stdint.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Structure.
///
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

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Create a Semaphore.
///
PLUTO_Semaphore_t PLUTO_CreateSemaphore(const char *path, const char *name);
///
/// \brief  Desctroy a Semaphore.
///
void PLUTO_DestroySemaphore(PLUTO_Semaphore_t *semaphore);
///
/// \brief  Decrement the Value 1.
///         Blocks if the Value was 0 and this Function was called until this Function is able to decrement the Value by 1.
///
PLUTO_SEM_ReturnValue_t PLUTO_SemaphoreWait(PLUTO_Semaphore_t semaphore); 
///
/// \brief  Inkrement the Value by 1.
///
PLUTO_SEM_ReturnValue_t PLUTO_SemaphoreSignal(PLUTO_Semaphore_t semaphore); 
///
/// \brief  Get the current Value.
///
int32_t PLUTO_SemaphoreValue(PLUTO_Semaphore_t semaphore); 

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
