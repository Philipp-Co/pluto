#include "pluto/os_abstraction/pluto_logger.h"
#include <pluto/os_abstraction/signals/pluto_signal.h>
#include <pluto/os_abstraction/pluto_time.h>

#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>

#define PLUTO_OS_ABSTRACTION_SIG_QUEUE_SIZE 64
#define PLUTO_OS_ABSTRACTION_SIG_QUEUE_SIZE_MASK (PLUTO_OS_ABSTRACTION_SIG_QUEUE_SIZE - 1)

struct PLUTO_SignalListElement
{
    PLUTO_Time_t timestamp;
    int signum;
    pid_t initiating_process;
} __attribute__((aligned(64)));

struct PLUTO_SignalRingbuffer
{
    struct PLUTO_SignalListElement buffer[PLUTO_OS_ABSTRACTION_SIG_QUEUE_SIZE];
    atomic_uint write_index;
    atomic_uint read_index;
} __attribute__((aligned(64)));

struct PLUTO_SignalHandler
{
    struct PLUTO_SignalRingbuffer ringbuffer;
    PLUTO_Logger_t logger;
} __attribute__((aligned(64)));

static struct PLUTO_SignalHandler PLUTO_global_SignalHandler = {
    .ringbuffer = {
        .read_index=0,
        .write_index=0
    }
};

static void PLUTO_SigactionHandler(int signo, siginfo_t *info, void *context);

PLUTO_SignalHandler_t PLUTO_CreateSignalHandler(PLUTO_Logger_t logger)
{
    PLUTO_global_SignalHandler.logger = logger;
    return &PLUTO_global_SignalHandler;
}

bool PLUTO_SignalAddListener(PLUTO_SignalHandler_t object, const PLUTO_Signal_t signal)
{
    struct sigaction act = { 0 };
    act.sa_flags = SA_SIGINFO; //| SA_UNSUPPORTED | SA_EXPOSE_TAGBITS;
    act.sa_sigaction = &PLUTO_SigactionHandler;
    if (sigaction(signal.signal_numer, &act, NULL) == -1) {
        PLUTO_LoggerWarning(object->logger, "Unable to add Listener for Signal %i", signal.signal_numer);
        return false;
    }
    PLUTO_LoggerDebug(
        object->logger,
        "Sucessfully registered a Signalhandler for Signal %i",
        signal.signal_numer
    );
    return true;
}

static void PLUTO_EmptySigactionHandler(int signo, siginfo_t *info, void *context)
{
    (void)signo;
    (void)info;
    (void)context;
}

void PLUTO_SignalRemoveListener(PLUTO_SignalHandler_t object, const PLUTO_Signal_t signal)
{
    struct sigaction act = {0};
    act.sa_sigaction = &PLUTO_EmptySigactionHandler;
    if (sigaction(signal.signal_numer, &act, NULL) == -1)
    {
        PLUTO_LoggerWarning(object->logger, "Unable to add Listener for Signal %i", signal.signal_numer);    
    }
}

void PLUTO_DestroySignal(PLUTO_SignalHandler_t *object)
{
    assert(NULL != object);
    assert(NULL != (*object));
    *object = NULL;
}

static void PLUTO_SigactionHandler(int signo, siginfo_t *info, void *context)
{
    //
    // https://man7.org/linux/man-pages/man2/sigaction.2.html
    //
    (void)context;
    unsigned int write_index = atomic_fetch_add(&PLUTO_global_SignalHandler.ringbuffer.write_index, 1) & PLUTO_OS_ABSTRACTION_SIG_QUEUE_SIZE_MASK; 
    PLUTO_global_SignalHandler.ringbuffer.buffer[write_index].signum = signo;
    PLUTO_global_SignalHandler.ringbuffer.buffer[write_index].initiating_process = info->si_pid;
    PLUTO_global_SignalHandler.ringbuffer.buffer[write_index].timestamp = PLUTO_TimeNow();
}

bool PLUTO_SignalPendingEvent(PLUTO_SignalHandler_t handler, PLUTO_SignalEvent_t *event)
{
    unsigned int read_index = atomic_load(&handler->ringbuffer.read_index) & PLUTO_OS_ABSTRACTION_SIG_QUEUE_SIZE_MASK;
    if(atomic_load(&handler->ringbuffer.write_index) == read_index)
    {
        return false;
    }
    read_index = atomic_fetch_add(&handler->ringbuffer.read_index, 1) & PLUTO_OS_ABSTRACTION_SIG_QUEUE_SIZE_MASK;
    event->signum = handler->ringbuffer.buffer[read_index].signum;
    event->initiating_process = handler->ringbuffer.buffer[read_index].initiating_process;
    event->timestamp = handler->ringbuffer.buffer[read_index].timestamp;
    return true;
}

void PLUTO_SignalSuspend(void)
{
    PLUTO_LoggerInfo(PLUTO_global_SignalHandler.logger, "Suspend Process, waiting for Signals to arrive.");
    sigset_t sigset = {0};
    (void)sigsuspend(&sigset);
}
