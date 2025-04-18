#include <pluto/os_abstraction/test/test_pluto_time.h>
#include <pluto/os_abstraction/pluto_time.h>

#include <stdio.h>
#include <Unity/src/unity.h>


void PLUTO_TEST_TimeInitial(void)
{
    PLUTO_Time_t time = PLUTO_TimeNow();
    (void)time;
}


void PLUTO_TEST_TimeFromString(void)
{
    PLUTO_Time_t time = PLUTO_TimeFromString(
        "2025-01-02T23:59:32.999"
    );

    TEST_ASSERT_EQUAL_INT(
        2025 - 1900,
        time.time.tm_year
    );
    TEST_ASSERT_EQUAL_INT( 
        1,
        time.time.tm_mon + 1
    );
    TEST_ASSERT_EQUAL_INT( 
        2,
        time.time.tm_mday
    );

    TEST_ASSERT_EQUAL_INT(
        23,
        time.time.tm_hour
    );
    TEST_ASSERT_EQUAL_INT(
        59,
        time.time.tm_min
    );
    TEST_ASSERT_EQUAL_INT(
        32,
        time.time.tm_sec
    );

    TEST_ASSERT_EQUAL_INT(
        999,
        time.milliseconds
    );
}

void PLUTO_TEST_TimeToString(void)
{
    PLUTO_Time_t time = PLUTO_TimeFromString(
        "2025-01-02T23:59:32.999"
    );
    char buffer[1024];
    PLUTO_TimeToString(
        time,
        buffer, sizeof(buffer)
    );

    TEST_ASSERT_EQUAL_STRING(
        "2025-01-02T23:59:32.999",
        buffer
    );
}

