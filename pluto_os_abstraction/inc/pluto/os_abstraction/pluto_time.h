#ifndef __PLUTO_OS_ABSTRACTION_TIME_H__
#define __PLUTO_OS_ABSTRACTION_TIME_H__

#include <pluto/os_abstraction/config/pluto_function_attributes.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>


typedef struct
{
    //
    // https://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html
    // Size on MacOs:
    //  9 * sizeof(int)     9 * 4       9 * 4
    //  1 * sizeof(long)    1 * 4       1 * 8
    //  1 * sizeof(char*)   1 * 8       1 * 8
    //                      ------------------
    //                      48          52
    //
    //struct tm time; 
    // ---
    /*
    union 
    {
        struct
        {
            uint32_t year           : 28;   // year since 1900  16      2
            uint32_t month          : 4;    // 0 - 11           4       1
            uint32_t day            : 5;    // 0 - 31           5       1
            uint32_t hour           : 5;    // 0 - 23           5       1
            uint32_t minutes        : 6;    // 0 - 59           6       1
            uint32_t seconds        : 6;    // 0 - 60           6       1
            uint32_t milliseconds   : 10;   // 0 - 999          10      2
        } as_time;
        uint64_t as_uint64;
    };
    */
    uint64_t time;
} PLUTO_Time_t __attribute__((aligned(8)));

void PLUTO_TimeSet(PLUTO_Time_t *time, uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minutes, uint8_t seconds, uint32_t milliseconds);
uint32_t PLUTO_TimeYear(PLUTO_Time_t time) PLUTO_FUNCTION_INLINE;
uint32_t PLUTO_TimeMonth(PLUTO_Time_t time) PLUTO_FUNCTION_INLINE;
uint32_t PLUTO_TimeDay(PLUTO_Time_t time) PLUTO_FUNCTION_INLINE;
uint32_t PLUTO_TimeHour(PLUTO_Time_t time) PLUTO_FUNCTION_INLINE;
uint32_t PLUTO_TimeMinutes(PLUTO_Time_t time) PLUTO_FUNCTION_INLINE;
uint32_t PLUTO_TimeSeconds(PLUTO_Time_t time) PLUTO_FUNCTION_INLINE;
uint32_t PLUTO_TimeMilliseconds(PLUTO_Time_t time) PLUTO_FUNCTION_INLINE;

///
/// \brief  Create a Timestamp for "now".
///         The Timestamp is calculated for UTC.
///
PLUTO_Time_t PLUTO_TimeNow(void) PLUTO_FUNCTION_INLINE;
///
/// \brief  Convert a String to a Timestamp.
///         The String must be formatted %Y-%m-%dT%H:%M:%S.%u
///         With
///         %Y -- Year with Century -- 2025
///         %m -- Month             -- 0 - 11
///         %d -- Day               -- 1 - 31
///         %H -- Hour of Day       -- 0 - 23
///         %M -- Minute of Hour    -- 0 - 59
///         %S -- Second            -- 0 - 59
///         %u -- Millisecond       -- 0 - 999
///
///         This Module assumes, that all given Timestamps are in UTC,
///         therefore no Timezone is encoded into the String.
///
/// \example A valid Timestamp is "2025-01-01T23:59:32.999".
///
PLUTO_Time_t PLUTO_TimeFromString(const char *buffer);
///
/// \brief  Converts a Timestamp to a String.  
///         The String will be formatted %Y-%m-%dT%H:%M:%S.%u
///         
///         This Module assumes, that all given Timestamps are in UTC,
///         therefore no Timezone is encoded into the String.
///
bool PLUTO_TimeToString(PLUTO_Time_t time, char *buffer, size_t nbytes);

bool PLUTO_TimeSmallerThan(const PLUTO_Time_t a, const PLUTO_Time_t b);
#endif
