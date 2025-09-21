
#include <pluto/os_abstraction/pluto_time.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

static uint64_t PLUTO_TimeMakeYear(int time);
static uint64_t PLUTO_TimeMakeMonth(int time);
static uint64_t PLUTO_TimeMakeDay(int time);
static uint64_t PLUTO_TimeMakeHour(int time);
static uint64_t PLUTO_TimeMakeMinutes(int time);
static uint64_t PLUTO_TimeMakeSeconds(int time);
static uint64_t PLUTO_TimeMakeMilliseconds(int time);

static uint64_t PLUTO_TimeMakeTime(const struct tm *t, int millis);


void PLUTO_TimeSet(PLUTO_Time_t *time, uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minutes, uint8_t seconds, uint32_t milliseconds)
{
    time->time =  PLUTO_TimeMakeYear((int)year) 
        | PLUTO_TimeMakeMonth((int)month) 
        | PLUTO_TimeMakeDay((int)day) 
        | PLUTO_TimeMakeHour((int)hour)
        | PLUTO_TimeMakeMinutes((int)minutes)
        | PLUTO_TimeMakeSeconds((int)seconds)
        | PLUTO_TimeMakeMilliseconds((int)milliseconds); 
}

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
        .time = PLUTO_TimeMakeTime(nowtm, (int)(tv.tv_usec / 1000)) 
    };
    return time;
}

PLUTO_Time_t PLUTO_TimeFromString(const char *buffer)
{
    //
    // https://man7.org/linux/man-pages/man3/strptime.3.html
    //
    int milliseconds = 0;
    struct tm tm_time = {0};
    const char* millis = strptime(buffer, "%Y-%m-%dT%H:%M:%S", &tm_time);
    if(!millis)
    {
        // pass...
    }
    else
    {
        char *end;
        milliseconds = strtol(++millis, &end, 10);
        if(end == millis)
        {
            milliseconds = 0;
        }
    }
    PLUTO_Time_t time = {
        .time = PLUTO_TimeMakeTime(&tm_time, milliseconds) 
    };
    return time;
}

bool PLUTO_TimeToString(PLUTO_Time_t time, char *buffer, size_t nbytes)
{
    //printf("Time: tm_sec %i\n", time.time.tm_sec);
    char timebuffer[256];
    struct tm tm_time = {
        .tm_sec=(int)PLUTO_TimeSeconds(time),
        .tm_min=(int)PLUTO_TimeMinutes(time),
        .tm_hour=(int)PLUTO_TimeHour(time),
        .tm_mday=(int)PLUTO_TimeDay(time),
        .tm_mon=(int)PLUTO_TimeMonth(time),
        .tm_year=(int)PLUTO_TimeYear(time),
        .tm_wday=0,
        .tm_yday=0,
        .tm_isdst=0,
        .tm_gmtoff=0,
        .tm_zone="UTC"
    };
    const int result = strftime(timebuffer, sizeof(timebuffer), "%Y-%m-%dT%H:%M:%S", &tm_time);
    if(result == 0)
    {
        return false;
    }
    snprintf(buffer, nbytes, "%s.%u", timebuffer, (unsigned int)PLUTO_TimeMilliseconds(time));
    return true;
}

bool PLUTO_TimeSmallerThan(const PLUTO_Time_t a, const PLUTO_Time_t b)
{
    return a.time < b.time;
}

static uint64_t PLUTO_TimeMakeYear(int time)
{
    return (((uint64_t)time) & 0x000000000FFFFFFFU) << 36U;
}

static uint64_t PLUTO_TimeMakeMonth(int time)
{
   return (((uint64_t)time) & 0x000000000000000FU) << 32U;
}

static uint64_t PLUTO_TimeMakeDay(int time)
{
    return (((uint64_t)time) & 0x000000000000001FU) << 27U;
}

static uint64_t PLUTO_TimeMakeHour(int time)
{
    return (((uint64_t)time) & 0x000000000000001FU) << 22U;
}

static uint64_t PLUTO_TimeMakeMinutes(int time)
{
    return (((uint64_t)time) & 0x000000000000003FU) << 16U;
}

static uint64_t PLUTO_TimeMakeSeconds(int time)
{
    return (((uint64_t)time) & 0x000000000000003FU) << 10U;
}

static uint64_t PLUTO_TimeMakeMilliseconds(int time)
{
    return ((uint64_t)time) & 0x00000000000003FFU;
}

static uint64_t PLUTO_TimeMakeTime(const struct tm *t, int millis)
{
    return PLUTO_TimeMakeYear(t->tm_year) 
        | PLUTO_TimeMakeMonth(t->tm_mon) 
        | PLUTO_TimeMakeDay(t->tm_mday) 
        | PLUTO_TimeMakeHour(t->tm_hour)
        | PLUTO_TimeMakeMinutes(t->tm_min)
        | PLUTO_TimeMakeSeconds(t->tm_sec)
        | PLUTO_TimeMakeMilliseconds(millis); 
}

uint32_t PLUTO_TimeYear(PLUTO_Time_t time)
{
    return (uint32_t)(time.time >> 36U);
}

uint32_t PLUTO_TimeMonth(PLUTO_Time_t time)
{
    return (uint32_t)((time.time >> 32U) & 0x000000000000000FU);
}

uint32_t PLUTO_TimeDay(PLUTO_Time_t time)
{
    return (uint32_t)((time.time >> 27) & 0x000000000000001FU);
}
uint32_t PLUTO_TimeHour(PLUTO_Time_t time)
{
    return (uint32_t)((time.time >> 22U) & 0x000000000000001FU);
}

uint32_t PLUTO_TimeMinutes(PLUTO_Time_t time)
{
    return (uint32_t)((time.time >> 16U) & 0x000000000000003FU);
}

uint32_t PLUTO_TimeSeconds(PLUTO_Time_t time)
{
    return (uint32_t)((time.time >> 10U) & 0x000000000000003FU);
}

uint32_t PLUTO_TimeMilliseconds(PLUTO_Time_t time)
{
    return (uint32_t)(time.time & 0x00000000000003FFU);
}
