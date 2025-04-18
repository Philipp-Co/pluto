#include "Unity/src/unity_internals.h"
#include <Unity/src/unity.h>
#include <pluto/pluto_edge/test/test_pluto_edge.h>
#include <pluto/os_abstraction/pluto_message_queue.h>


static const char *path = "/tmp/";
static const char *name = "edge_test";
static PLUTO_MessageQueue_t PLUTO_TEST_queue = NULL;
static PLUTO_Logger_t PLUTO_TEST_logger = NULL;

void setUp(void);
void tearDown(void);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    PLUTO_TEST_logger = PLUTO_CreateLogger("test-edge");

    UNITY_BEGIN();
    RUN_TEST(PLUTO_TEST_EdgeInitial); 
    RUN_TEST(PLUTO_TEST_EdgeSendAndReceive); 

    PLUTO_DestroyLogger(&PLUTO_TEST_logger);
    return UNITY_END();
}

void setUp(void)
{
    PLUTO_TEST_queue = PLUTO_CreateMessageQueue(
        path,
        name,
        0777,
        PLUTO_TEST_logger
    );
}   

void tearDown(void)
{
    PLUTO_DestroyMessageQueue(&PLUTO_TEST_queue);
}
