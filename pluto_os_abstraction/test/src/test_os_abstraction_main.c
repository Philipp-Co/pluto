
#include <pluto/os_abstraction/test/test_pluto_time.h>
#include <pluto/os_abstraction/test/test_os_abstraction_messagequeue.h>
#include <pluto/os_abstraction/test/test_os_abstraction_semaphore.h>
#include <pluto/os_abstraction/test/test_system_events.h>
#include <pluto/os_abstraction/test/test_signals.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <Unity/src/unity.h>
#include <sys/stat.h>


void setUp(void);
void tearDown(void);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(PLUTO_TEST_TimeInitial);
    RUN_TEST(PLUTO_TEST_TimeFromString);
    RUN_TEST(PLUTO_TEST_TimeToString);

    RUN_TEST(PLUTO_TEST_SemaphoreInitialCreate);
    RUN_TEST(PLUTO_TEST_SemaphoreInitialGet);

    RUN_TEST(PLUTO_TEST_MessageQueueInitialCreate);
    RUN_TEST(PLUTO_TEST_MessageQueueInitialGet);
    RUN_TEST(PLUTO_TEST_MessageQueueSendAndRead);

    RUN_TEST(PLUTO_TEST_SystemEventsInitial);
    RUN_TEST(PLUTO_TEST_SystemEventsFileEvent);
    RUN_TEST(PLUTO_TEST_SystemEventsDeregisterFileObserver);
    
    RUN_TEST(PLUTO_TEST_SignalsInitial);
    RUN_TEST(PLUTO_TEST_SignalsAddListener);
    RUN_TEST(PLUTO_TEST_SignalsRemoveListener);

    return UNITY_END();
}

void setUp(void)
{
    mkdir(PLUTO_TEST_RESSOURCE_DIR, 0777);
#if defined(PLUTO_TEST)
    PLUTO_MallocResetState();
#endif
}

void tearDown(void)
{
#if defined(PLUTO_TEST)
    TEST_ASSERT_TRUE(PLUTO_MallocCountEqual());
    PLUTO_MallocResetState();
#endif
}
