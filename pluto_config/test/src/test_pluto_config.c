
#include "pluto/os_abstraction/pluto_logger.h"
#include <test_node_config.h>
#include <test_core_config.h>
#include <test_pluto_config.h>

#include <Unity/src/unity.h>

PLUTO_Logger_t TEST_PLUTO_Logger = NULL;

void setUp(void);
void tearDown(void);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(test_initial);
    RUN_TEST(test_minimal_config);
    RUN_TEST(test_multiple_output_queues_config);
    RUN_TEST(test_python_config);
    RUN_TEST(test_core_initial);
    RUN_TEST(test_core_multiple_nodes);
    return UNITY_END();
}

void setUp(void)
{
    TEST_PLUTO_Logger = PLUTO_CreateLogger("Test Config");
}

void tearDown(void)
{
    PLUTO_DestroyLogger(&TEST_PLUTO_Logger);
}

