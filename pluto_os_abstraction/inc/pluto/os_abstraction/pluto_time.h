#ifndef __PLUTO_OS_ABSTRACTION_TIME_H__
#define __PLUTO_OS_ABSTRACTION_TIME_H__

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
    //
    struct tm time;
    uint32_t milliseconds;
} PLUTO_Time_t;

///
/// \brief  Create a Timestamp for "now".
///         The Timestamp is calculated for UTC.
///
PLUTO_Time_t PLUTO_TimeNow(void);
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
