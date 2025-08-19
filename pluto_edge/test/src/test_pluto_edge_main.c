#include "Unity/src/unity_internals.h"
#include <Unity/src/unity.h>
#include <pluto/pluto_edge/test/test_pluto_edge.h>
#include <pluto/os_abstraction/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_malloc.h>


static const char *path = "/tmp/";
static const char *name = "edge_test";

void setUp(void);
void tearDown(void);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(PLUTO_TEST_EdgeInitial); 
    RUN_TEST(PLUTO_TEST_EdgeSendAndReceive); 
    return UNITY_END();
}

void setUp(void)
{
#if defined(PLUTO_TEST)
    PLUTO_MallocResetState();
    PLUTO_TEST_logger = PLUTO_CreateLogger("test-edge");
#endif
    printf("--- setup --\n");
    PLUTO_TEST_queue = PLUTO_CreateMessageQueue(
        path,
        name,
        0777,
        PLUTO_TEST_logger
    );
    printf("--- setup ende --\n");
}   

void tearDown(void)
{
    PLUTO_DestroyMessageQueue(&PLUTO_TEST_queue);
    PLUTO_DestroyLogger(&PLUTO_TEST_logger);
#if defined(PLUTO_TEST)
    TEST_ASSERT_TRUE(PLUTO_MallocCountEqual());
    PLUTO_MallocResetState();
#endif
}


PLUTO_Logger_t PLUTO_TEST_logger = NULL;
PLUTO_MessageQueue_t PLUTO_TEST_queue = NULL;
