#include "Unity/src/unity.h"
#include <pluto/os_abstraction/test/test_os_abstraction_semaphore.h>
#include <pluto/os_abstraction/pluto_semaphore.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/pluto_malloc.h>


void PLUTO_TEST_SemaphoreInitialCreate(void)
{
    //
    // Create Path from Compile Definition.
    // Defined from CMakeLists.txt
    //
    char *path = (char*)PLUTO_Malloc(4096);
    const char *name = "sem-test";
    snprintf(
        path,
        4096,
        "%stemp/",
        TEST_PLUTO_WORKDIR
    );
    //
    // ------------------------------------
    //
    PLUTO_Logger_t logger = PLUTO_CreateLogger("PLUTO_TEST_SemaphoreInitialCreate");
    PLUTO_Semaphore_t sem = PLUTO_CreateSemaphore(
        path,
        name,
        logger
    );
    
    PLUTO_Free(path);
    PLUTO_DestroySemaphore(&sem);
    PLUTO_DestroyLogger(&logger);
}

void PLUTO_TEST_SemaphoreInitialGet(void)
{
    //
    // Create Path from Compile Definition.
    // Defined from CMakeLists.txt
    //
    char *path = (char*)PLUTO_Malloc(4096);
    const char *name = "sem-test";
    snprintf(
        path,
        4096,
        "%stemp/",
        TEST_PLUTO_WORKDIR
    );
    //
    // ------------------------------------
    //
    PLUTO_Logger_t logger = PLUTO_CreateLogger("PLUTO_TEST_SemaphoreInitialGet");
    printf("------\n");
    PLUTO_Semaphore_t sem = PLUTO_CreateSemaphore(
        path,
        name,
        logger
    );
    printf("------\n");
    TEST_ASSERT_NOT_NULL(sem);
    
    PLUTO_Semaphore_t sem_get = PLUTO_SemaphoreGet(
        path, name, logger
    );
    printf("------\n");
    TEST_ASSERT_NOT_NULL(sem_get);

    PLUTO_Free(path);
    PLUTO_DestroySemaphore(&sem_get);
    PLUTO_DestroySemaphore(&sem);
    PLUTO_DestroyLogger(&logger);
}
