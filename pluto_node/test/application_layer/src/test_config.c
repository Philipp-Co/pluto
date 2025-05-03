
#include "Unity/src/unity.h"
#include "Unity/src/unity_internals.h"
#include <pluto/application_layer/pluto_config.h>
#include <pluto/os_abstraction_layer/pluto_malloc.h>
#include <pluto/test/test_config.h>


void PLUTO_TEST_ConfigInitial(void)
{
    char *filename = "/Users/philippkroll/Repositories/pluto/build/test.cfg";
    char *name = "test"; 
    PLUTO_Config_t config = PLUTO_CreateConfig(filename, name);
    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_EQUAL_INT(0, PLUTO_MallocCountEqual());
}
