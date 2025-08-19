
#include "pluto/pluto_config/pluto_config.h"
#include <pluto/pluto_node/test/test_processor.h>
#include <pluto/pluto_node/test/prepare.h>
#include <pluto/os_abstraction/pluto_malloc.h>
#include <pluto/pluto_edge/pluto_edge.h>
#include <pluto/pluto_node/test/test_python.h>
#include <Unity/src/unity.h>
#include <stdlib.h>


void setUp(void);
void tearDown(void);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    
    RUN_TEST(PLUTO_TEST_ProcessorInitial);
    RUN_TEST(PLUTO_TEST_ProcessorProcessWithEmptyPayload);
    RUN_TEST(PLUTO_TEST_ProcessorProcessWithStandardPayload);
    RUN_TEST(PLUTO_TEST_ProcessorProcessWithBigPayload);
    RUN_TEST(PLUTO_TEST_ProcessorProcessWithMultipleMessages);
    RUN_TEST(PLUTO_TEST_PythonInitial);
    RUN_TEST(PLUTO_TEST_PythonProcessEventWhenQueueIsEmpty);
    RUN_TEST(PLUTO_TEST_PythonProcessEventWhenQueueIsNotEmpty);

    return UNITY_END();
}

void setUp(void)
{
#if defined(PLUTO_TEST)
    PLUTO_MallocResetState();
#endif
    PLUTO_TEST_edge_logger = PLUTO_CreateLogger("test-edge");
    PLUTO_TEST_processor_logger = PLUTO_CreateLogger("test-processor");

    PLUTO_LoggerInfo(PLUTO_TEST_processor_logger, "Use Nodeconfig: %s", PLUTO_TEST_CONFIG);

    PLUTO_TEST_config = PLUTO_CreateConfig(
        PLUTO_TEST_CONFIG,
        PLUTO_TEST_name,
        PLUTO_TEST_processor_logger
    );

    TEST_ASSERT_NOT_NULL(
        PLUTO_TEST_config
    );
}

void tearDown(void)
{
    PLUTO_DestroyConfig(&PLUTO_TEST_config);
    PLUTO_DestroyLogger(&PLUTO_TEST_edge_logger);
    PLUTO_DestroyLogger(&PLUTO_TEST_processor_logger);
    
#if defined(PLUTO_TEST)
    // Memory Check...
    TEST_ASSERT_TRUE(PLUTO_MallocCountEqual());
    PLUTO_MallocResetState();
#endif
}

const char* PLUTO_TEST_name = "pluto-0_iq";
PLUTO_Logger_t PLUTO_TEST_edge_logger = NULL;
PLUTO_Logger_t PLUTO_TEST_processor_logger = NULL;
PLUTO_Config_t PLUTO_TEST_config = NULL;

#ifndef PLUTO_TEST_PYTHON_PATH
#define PLUTO_TEST_PYTHON_PATH "\"\";/Users/philippkroll/Repositories/pluto/pluto_node/test/ressources/python/;/opt/homebrew/Cellar/python@3.13/3.13.3/Frameworks/Python.framework/Versions/3.13/lib/python313.zip;/opt/homebrew/Cellar/python@3.13/3.13.3/Frameworks/Python.framework/Versions/3.13/lib/python3.13;/opt/homebrew/Cellar/python@3.13/3.13.3/Frameworks/Python.framework/Versions/3.13/lib/python3.13/lib-dynload;/opt/homebrew/lib/python3.13/site-packages\0"
#endif
#ifndef PLUTO_TEST_PYTHON_EXECUTABLE
#define PLUTO_TEST_PYTHON_EXECUTABLE "loopback"
#endif

const char *PLUTO_TEST_python_path = PLUTO_TEST_PYTHON_PATH;
const char *PLUTO_TEST_executable = PLUTO_TEST_PYTHON_EXECUTABLE;
