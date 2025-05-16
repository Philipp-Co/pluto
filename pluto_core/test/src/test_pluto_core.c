
#include <pluto/pluto_core/test/test_pluto_node_state.h>
#include <pluto/pluto_core/test/test_pluto_core_state.h>

#include "Unity/src/unity_internals.h"
#include <Unity/src/unity.h>


void setUp(void);
void tearDown(void);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(PLUTO_TEST_CoreNodeStateInitial);
    RUN_TEST(PLUTO_TEST_CoreStateInitial);
    return UNITY_END();
}

void setUp(void)
{

}

void tearDown(void)
{

}
