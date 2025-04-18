///
/// \brief  This Module contains the Interfacedefinition for a Semaphore.
///
#ifndef __PLUTO_SEMAPHORE_H__
#define __PLUTO_SEMAPHORE_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/pluto_types.h>
#include <pluto/os_abstraction/pluto_logger.h>

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
    PLUTO_Logger_t logger;
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
///         The Object is created OS global, which means other Processes can see and use this Object as well.
///
PLUTO_Semaphore_t PLUTO_CreateSemaphore(const char *path, const char *name, PLUTO_Logger_t logger);
///
/// \brief  Get a Semaphore.
///         A call to this Function does not create a Semaphore, but fails if the requestd Object does not exist.
///
PLUTO_Semaphore_t PLUTO_SemaphoreGet(const char *path, const char *name, PLUTO_Logger_t logger);
///
/// \brief  Destroy a Semaphore.
///         The Semaphores local representation is destroy. Destruction of the global Object depends on the Semaphores Value.
///         If the Value is equal to 0 then it is destroyed globally.
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
