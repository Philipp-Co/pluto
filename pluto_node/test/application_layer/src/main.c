
#include <pluto/test/test_config.h>
#include <Unity/src/unity.h>

void setUp(void);
void tearDown(void);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(PLUTO_TEST_ConfigInitial);
    return UNITY_END();
}

void setUp(void)
{

}

void tearDown(void)
{

}
