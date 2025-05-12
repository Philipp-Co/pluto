#ifndef __PLUTO_OS_ABSTRACTION_LOGGER_H__
#define __PLUTO_OS_ABSTRACTION_LOGGER_H__


#define PLUTO_LOG_LEVEL_DEBUG   2000
#define PLUTO_LOG_LEVEL_INFO    1000
#define PLUTO_LOG_LEVEL_WARNING 500
#define PLUTO_LOG_LEVEL_ERROR   100

#ifndef PLUTO_LOG_LEVEL
#define PLUTO_LOG_LEVEL PLUTO_LOG_LEVEL_DEBUG
#endif
#define PLUTO_LOGGER_ACTIVE (PLUTO_LOG_LEVEL >= PLUTO_LOG_LEVEL_ERROR)


struct PLUTO_Logger;
typedef struct PLUTO_Logger* PLUTO_Logger_t;

PLUTO_Logger_t PLUTO_CreateLogger(const char *name);
void PLUTO_DestroyLogger(PLUTO_Logger_t *logger);

void PLUTO_LoggerDebug(PLUTO_Logger_t logger, const char *format, ...);
void PLUTO_LoggerInfo(PLUTO_Logger_t logger, const char *format, ...);
void PLUTO_LoggerWarning(PLUTO_Logger_t logger, const char *format, ...);
void PLUTO_LoggerError(PLUTO_Logger_t logger, const char *format, ...);

#endif
