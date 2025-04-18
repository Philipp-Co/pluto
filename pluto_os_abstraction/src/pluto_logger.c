
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/pluto_malloc.h>
#include <pluto/os_abstraction/pluto_time.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>


struct PLUTO_Logger
{
    FILE *file_descriptor;
    char *name;
};


PLUTO_Logger_t PLUTO_CreateLogger(const char *name)
{
    PLUTO_Logger_t logger = PLUTO_Malloc(
        sizeof(struct PLUTO_Logger)
    );
    logger->name = PLUTO_Malloc(strlen(name) + 1); 
    memset(logger->name, '\0', strlen(name) + 1);
    memcpy(logger->name, name, strlen(name) + 1);
    logger->file_descriptor = stdout;
    return logger; 
}

void PLUTO_DestroyLogger(PLUTO_Logger_t *logger)
{
    PLUTO_Free(*logger);
}

void PLUTO_LoggerDebug(PLUTO_Logger_t logger, const char *format, ...)
{
#if PLUTO_LOG_LEVEL >= PLUTO_LOG_LEVEL_DEBUG
    char *buffer = PLUTO_Malloc(4096);
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(buffer, 4096, format, arglist);
    char *buffer1 = PLUTO_Malloc(8192);

    char timebuffer[128];
    PLUTO_Time_t time = PLUTO_TimeNow();
    PLUTO_TimeToString(time, timebuffer, sizeof(timebuffer));

    snprintf(buffer1, 8192, "[%s][DEBUG][%s] - %s\n", timebuffer, logger->name, buffer);
    va_end(arglist);
#else
    (void)logger;
    (void)format;
#endif
}

void PLUTO_LoggerInfo(PLUTO_Logger_t logger, const char *format, ...)
{
#if PLUTO_LOG_LEVEL >= PLUTO_LOG_LEVEL_INFO
    char *buffer = PLUTO_Malloc(4096);
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(buffer, 4096, format, arglist);
    char *buffer1 = PLUTO_Malloc(8192);

    char timebuffer[128];
    PLUTO_Time_t time = PLUTO_TimeNow();
    PLUTO_TimeToString(time, timebuffer, sizeof(timebuffer));

    snprintf(buffer1, 8192, "[%s][INFO ][%s] - %s\n", timebuffer, logger->name, buffer);
    va_end(arglist);

    fprintf(logger->file_descriptor, "%s", buffer1);

    PLUTO_Free(buffer);
    PLUTO_Free(buffer1);
#else
    (void)logger;
    (void)format;
#endif
}

void PLUTO_LoggerWarning(PLUTO_Logger_t logger, const char *format, ...)
{
#if PLUTO_LOG_LEVEL >= PLUTO_LOG_LEVEL_WARNING
    char *buffer = PLUTO_Malloc(4096);
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(buffer, 4096, format, arglist);
    char *buffer1 = PLUTO_Malloc(8192);
    
    char timebuffer[128];
    PLUTO_Time_t time = PLUTO_TimeNow();
    PLUTO_TimeToString(time, timebuffer, sizeof(timebuffer));
    
    snprintf(buffer1, 8192, "[%s][WARN ][%s] - %s\n", timebuffer, logger->name, buffer);
    va_end(arglist);

    fprintf(logger->file_descriptor, "%s", buffer1);

    PLUTO_Free(buffer);
    PLUTO_Free(buffer1);
#else
    (void)logger;
    (void)format;
#endif
}

void PLUTO_LoggerError(PLUTO_Logger_t logger, const char *format, ...)
{
#if PLUTO_LOG_LEVEL >= PLUTO_LOG_LEVEL_ERROR
    char *buffer = PLUTO_Malloc(4096);
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(buffer, 4096, format, arglist);
    char *buffer1 = PLUTO_Malloc(8192);
    
    char timebuffer[128];
    PLUTO_Time_t time = PLUTO_TimeNow();
    PLUTO_TimeToString(time, timebuffer, sizeof(timebuffer));
    
    snprintf(buffer1, 8192, "[%s][ERROR][%s] - %s\n", timebuffer, logger->name, buffer);
    va_end(arglist);

    fprintf(logger->file_descriptor, "%s", buffer1);

    PLUTO_Free(buffer);
    PLUTO_Free(buffer1);
#else
    (void)logger;
    (void)format;
#endif
}
