//
// --------------------------------------------------------------------------------------------------------------------
//
#include "pluto/os_abstraction/pluto_logger.h"
#include <pluto/os_abstraction/system_events/pluto_system_events.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
//
// kqueue
//
#include <sys/event.h>
#include <unistd.h>
#include <fcntl.h>
//
// --------------------------------------------------------------------------------------------------------------------
//
PLUTO_SystemEvent_t PLUTO_CreateSystemEvent(void)
{
    PLUTO_SystemEvent_t event = (PLUTO_SystemEvent_t)PLUTO_Malloc(sizeof(struct PLUTO_SystemEvent));
    event->descriptor = -1;
    event->timestamp = PLUTO_TimeNow();
    return event;
}

void PLUTO_DestroySystemEvent(PLUTO_SystemEvent_t *event)
{
    PLUTO_Free(*event);
    *event = NULL;
}

int32_t PLUTO_SystemEventFiledescriptor(PLUTO_SystemEvent_t event)
{
    return event->descriptor;
}

struct PLUTO_SystemEventHandler
{
    PLUTO_Logger_t logger;
    int kqueue;
};

PLUTO_SystemEventHandler_t PLUTO_CreateSystemEventHandler(PLUTO_Logger_t logger)
{
    assert(NULL != logger);
    PLUTO_SystemEventHandler_t handler = (PLUTO_SystemEventHandler_t)PLUTO_Malloc(
        sizeof(struct PLUTO_SystemEventHandler)
    );
    handler->logger = logger;
    handler->kqueue = kqueue();
    if(handler->kqueue < 0)
    {
        PLUTO_LoggerWarning(logger, "Unable to create kqueue...");
        PLUTO_Free(handler);
        return NULL;
    }
    return handler;
}

void PLUTO_DestroySystemEventHandler(PLUTO_SystemEventHandler_t *handler)
{
    assert(NULL != *handler);    
    (void)close((*handler)->kqueue);
    PLUTO_Free(*handler);
    *handler = NULL;
}

int32_t PLUTO_SystemEventsHandlerRegisterObserver(PLUTO_SystemEventHandler_t handler, int descriptor)
{
    assert(NULL != handler);
    assert(NULL != handler->logger);
    PLUTO_LoggerInfo(
        handler->logger,
        "Register Observer for Filedescriptor %i on kqueue %i",
        descriptor,
        handler->kqueue
    );
    //descriptor = open("/Users/philippkroll/Repositories/pluto/plyto/test.txt", O_RDWR);
    struct kevent event; 
    EV_SET(
        &event, 
        descriptor, 
        EVFILT_VNODE, 
        EV_ADD | EV_CLEAR, 
        NOTE_WRITE | NOTE_DELETE | NOTE_LINK | NOTE_ATTRIB | NOTE_RENAME | NOTE_EXTEND,
	    0, 
        NULL
    );
        
    const int32_t result = (int32_t)kevent(
        handler->kqueue, 
        &event, 1, 
        NULL, 0, 
        NULL
    );
    if(result < 0)
    {
        PLUTO_LoggerWarning(
            handler->logger,
            "Error registering a System Event Observer: %i",
            result
        );
    }
    return result;
}

int32_t PLUTO_SystemEventsHandlerDeregisterObserver(PLUTO_SystemEventHandler_t handler, int descriptor)
{
    assert(NULL != handler);
    assert(NULL != handler->logger);
    PLUTO_LoggerInfo(
        handler->logger,
        "Deregister Observer for Filedescripto %i",
        descriptor
    );
    struct kevent event;
    EV_SET(
        &event,
        descriptor,
        EVFILT_VNODE,
        EV_DELETE,
        0,
        0,
        NULL
    );
    return kevent(handler->kqueue, &event, 1, NULL, 0, NULL);
}

int32_t PLUTO_SystemEventsPoll(PLUTO_SystemEventHandler_t handler, PLUTO_SystemEvent_t event) 
{
    assert(NULL != handler);
    assert(NULL != handler->logger);
    assert(handler->kqueue > 0);
    struct timespec timeout = {
        .tv_sec=0,
        .tv_nsec=0,//1000*1000*5
    };
    struct kevent kq_event;
    //PLUTO_LoggerInfo(handler->logger, "Read kqueue %i", handler->kqueue);
    const int32_t result = kevent(
        handler->kqueue,
        NULL, 0,
        &kq_event, 1,
       &timeout 
    );
    if(result > 0)
    {
        PLUTO_LoggerInfo(
            handler->logger,
            "Generate Event for Fd: %i",
            (int)kq_event.ident
        );
        event->descriptor = (int)kq_event.ident;
        event->timestamp = PLUTO_TimeNow();
        return PLUTO_SE_OK;
    }
    else if(result > 0 && (kq_event.flags & EV_ERROR))
    {
        PLUTO_LoggerWarning(handler->logger, "Polling kqueue %i: EV_ERROR", handler->kqueue);
        return PLUTO_SE_NO_EVENT;
    }
    else if(result < 0)
    {
        switch(errno)
        {
            case EINTR:
            case EAGAIN:
                return PLUTO_SE_NO_EVENT;
            default:
                break;
        }
        PLUTO_LoggerInfo(
            handler->logger,
            "Error Polling System Event: %i, %s",
            result,
            strerror(errno)
        );
        return PLUTO_SE_ERRROR;
    }
    return PLUTO_SE_NO_EVENT;
} 
//
// --------------------------------------------------------------------------------------------------------------------
//
