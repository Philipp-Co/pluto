
#include <pluto/pluto_core/test/test_pluto_node_state.h>
#include <pluto/pluto_core/test/test_pluto_core_state.h>
#include <pluto/pluto_core/test/prepare.h>

#include "Unity/src/unity_internals.h"
#include <Unity/src/unity.h>


void setUp(void);
void tearDown(void);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    PLUTO_TEST_GenerateCoreConfig(PLUTO_TEST_config_path);
    UNITY_BEGIN();
    RUN_TEST(PLUTO_TEST_CoreNodeStateInitial);
    RUN_TEST(PLUTO_TEST_CoreStateInitial);
    PLUTO_TEST_RemoveGeneratedCoreConfig(PLUTO_TEST_config_path);
    return UNITY_END();
}

void setUp(void)
{

}

void tearDown(void)
{

}
