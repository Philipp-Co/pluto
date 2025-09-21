#include <pluto/os_abstraction/test/test_pluto_time.h>
#include <pluto/os_abstraction/pluto_time.h>

#include <stdio.h>
#include <Unity/src/unity.h>


void PLUTO_TEST_TimeInitial(void)
{
    PLUTO_Time_t time = PLUTO_TimeNow();
    TEST_ASSERT_EQUAL(8, sizeof(time));
}

void PLUTO_TEST_TimeFromString(void)
{
    PLUTO_Time_t time = PLUTO_TimeFromString(
        "2025-01-02T23:59:32.999"
    );

    TEST_ASSERT_EQUAL_INT(
        2025 - 1900,
        PLUTO_TimeYear(time)
    );
    TEST_ASSERT_EQUAL_INT( 
        1,
        PLUTO_TimeMonth(time) + 1
    );
    TEST_ASSERT_EQUAL_INT( 
        2,
        PLUTO_TimeDay(time)
    );

    TEST_ASSERT_EQUAL_INT(
        23,
        PLUTO_TimeHour(time)
    );
    TEST_ASSERT_EQUAL_INT(
        59,
        PLUTO_TimeMinutes(time)
    );
    TEST_ASSERT_EQUAL_INT(
        32,
        PLUTO_TimeSeconds(time)
    );

    TEST_ASSERT_EQUAL_INT(
        999,
        PLUTO_TimeMilliseconds(time)
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

void PLUTO_TEST_TimeSmallerThen(void)
{
    static const char *greater[7] = {
        "2025-01-02T23:59:33.0",    // millis
        "2025-01-02T23:59:34.0",    // secs
        "2025-01-02T23:59:00.0",    // min
        "2025-01-02T23:59:00.0",    // h
        "2025-01-02T23:59:00.0",    // d
        "2025-02-02T23:59:00.0",    // m
        "2025-01-02T23:59:00.0"     // y
    };
    static const char *smaller[7] = {
        "2025-01-02T23:59:32.999",  // millis
        "2025-01-02T23:59:33.0",    // secs
        "2025-01-02T23:58:00.0",    // min
        "2025-01-02T22:59:00.0",    // h
        "2025-01-01T23:59:00.0",    // d
        "2025-01-02T23:59:00.0",    // m
        "2024-01-02T23:59:00.0"     // y
    };

    for(size_t i=0;i<7;++i)
    {
        PLUTO_Time_t time_a = PLUTO_TimeFromString(
            greater[i]
        );
        PLUTO_Time_t time_b = PLUTO_TimeFromString(
            smaller[i]
        );
        TEST_ASSERT_TRUE(PLUTO_TimeSmallerThan(time_b, time_a));
        TEST_ASSERT_FALSE(PLUTO_TimeSmallerThan(time_a, time_b));
    }
}
