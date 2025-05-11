
#include <pluto/os_abstraction/test/test_pluto_time.h>


#include <Unity/src/unity.h>


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
    return UNITY_END();
}

void setUp(void)
{

}

void tearDown(void)
{

}
