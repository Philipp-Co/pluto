#include <pluto/pluto_event/test/test_pluto_event.h>

#include <Unity/src/unity.h>


void setUp(void);
void tearDown(void);

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(PLUTO_TEST_TestEvent);
    RUN_TEST(PLUTO_TEST_EventFromBufferSuccess);
    RUN_TEST(PLUTO_TEST_EventFromBufferIdMissing);
    RUN_TEST(PLUTO_TEST_EventFromBufferEventMissing);
    RUN_TEST(PLUTO_TEST_EventFromBufferPayloadMissing);
    RUN_TEST(PLUTO_TEST_EventToString);
    return UNITY_END();
}

void setUp(void)
{

}

void tearDown(void)
{

}
