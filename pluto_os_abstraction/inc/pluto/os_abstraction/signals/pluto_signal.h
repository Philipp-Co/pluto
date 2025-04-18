#ifndef __PLUTO_OS_ABSTRACTION_SIGNAL_H__
#define __PLUTO_OS_ABSTRACTION_SIGNAL_H__

#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/pluto_time.h>
#include <stdbool.h>
#include <stdlib.h>

struct PLUTO_SignalHandler;
typedef struct PLUTO_SignalHandler* PLUTO_SignalHandler_t;

typedef struct
{
    int signal_numer;
} PLUTO_Signal_t;

typedef struct 
{
    PLUTO_Time_t timestamp;
    int signum;
    pid_t initiating_process;
} PLUTO_SignalEvent_t;

PLUTO_SignalHandler_t PLUTO_CreateSignalHandler(PLUTO_Logger_t logger);
bool PLUTO_SignalAddListener(PLUTO_SignalHandler_t object, const PLUTO_Signal_t signal);
void PLUTO_SignalRemoveListener(PLUTO_SignalHandler_t object, const PLUTO_Signal_t signal);
void PLUTO_DestroySignal(PLUTO_SignalHandler_t *object);

bool PLUTO_SignalPendingEvent(PLUTO_SignalHandler_t handler, PLUTO_SignalEvent_t *event);
void PLUTO_SignalSuspend(void);

#endif
