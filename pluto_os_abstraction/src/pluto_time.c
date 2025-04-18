
#include <pluto/os_abstraction/pluto_time.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>


PLUTO_Time_t PLUTO_TimeNow(void)
{
    struct timeval tv;
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    // Sometime not defined -.-' TIMESPEC_TO_TIMEVAL(&tv, &t);
    tv.tv_sec = t.tv_sec;
    tv.tv_usec = t.tv_nsec / 1000;
    
    time_t nowtime = tv.tv_sec;
    struct tm *nowtm = localtime(&nowtime);

    PLUTO_Time_t time = {
        .time = *nowtm,
        .milliseconds = tv.tv_usec / 1000
    };
    return time;
}

PLUTO_Time_t PLUTO_TimeFromString(const char *buffer)
{
    //
    // https://man7.org/linux/man-pages/man3/strptime.3.html
    //
    PLUTO_Time_t time = {
        .time = {0},
        .milliseconds = 0
    };
    const char* millis = strptime(buffer, "%Y-%m-%dT%H:%M:%S", &time.time);
    if(!millis)
    {
        memset(&time, 0, sizeof(time));
    }
    else
    {
        char *end;
        const int milliseconds = strtol(++millis, &end, 10);
        if(end == millis)
        {
            memset(&time, 0, sizeof(time));
        }
        else
        {
            time.milliseconds = milliseconds;
        }
    }
    return time;
}

bool PLUTO_TimeToString(PLUTO_Time_t time, char *buffer, size_t nbytes)
{
    //printf("Time: tm_sec %i\n", time.time.tm_sec);
    char timebuffer[256];
    const int result = strftime(timebuffer, sizeof(timebuffer), "%Y-%m-%dT%H:%M:%S", &time.time);
    if(result == 0)
    {
        return false;
    }
    snprintf(buffer, nbytes, "%s.%u", timebuffer, time.milliseconds);
    return true;
}

bool PLUTO_TimeSmallerThan(const PLUTO_Time_t a, const PLUTO_Time_t b)
{
    struct tm ta = a.time;
    struct tm tb = b.time;
    time_t seconds_since_epoch_a = mktime(&ta);
    time_t seconds_since_epoch_b = mktime(&tb);
    return (seconds_since_epoch_a * 1000 + a.milliseconds) < (seconds_since_epoch_b * 1000 + b.milliseconds);
}
