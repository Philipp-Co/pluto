#include "Unity/src/unity.h"
#include <pluto/os_abstraction/test/test_os_abstraction_semaphore.h>
#include <pluto/os_abstraction/pluto_semaphore.h>
#include <pluto/os_abstraction/pluto_logger.h>


void PLUTO_TEST_SemaphoreInitialCreate(void)
{
    const char *path = "/tmp/";
    const char *name = "sem-test";

    PLUTO_Logger_t logger = PLUTO_CreateLogger("PLUTO_TEST_SemaphoreInitialCreate");

    PLUTO_Semaphore_t sem = PLUTO_CreateSemaphore(
        path,
        name,
        logger
    );

    PLUTO_DestroySemaphore(&sem);
    PLUTO_DestroyLogger(&logger);
}

void PLUTO_TEST_SemaphoreInitialGet(void)
{
    const char *path = "/tmp/";
    const char *name = "sem-test";

    PLUTO_Logger_t logger = PLUTO_CreateLogger("PLUTO_TEST_SemaphoreInitialGet");

    PLUTO_Semaphore_t sem = PLUTO_CreateSemaphore(
        path,
        name,
        logger
    );
    TEST_ASSERT_NOT_NULL(sem);
    
    PLUTO_Semaphore_t sem_get = PLUTO_SemaphoreGet(
        path, name, logger
    );
    TEST_ASSERT_NOT_NULL(sem_get);

    PLUTO_DestroySemaphore(&sem_get);
    PLUTO_DestroySemaphore(&sem);
    PLUTO_DestroyLogger(&logger);
}
