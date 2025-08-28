//
// --------------------------------------------------------------------------------------------------------------------
//
#include "pluto/os_abstraction/pluto_logger.h"
#include <pluto/os_abstraction/system_events/pluto_system_events.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <assert.h>
#include <errno.h>
#include <string.h>

#if defined(PLUTO_KQUEUE_AVAILABLE) && PLUTO_KQUEUE_AVAILABLE
//
// kqueue
//
#include <sys/event.h>
#include <unistd.h>
#include <fcntl.h>

#else

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>

#endif

#if defined(PLUTO_KQUEUE_AVAILABLE) && PLUTO_KQUEUE_AVAILABLE
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

int32_t PLUTO_SystemEventHandlerRegisterFileObserver(PLUTO_SystemEventHandler_t handler, const char *path)
{
    (void)handler;
    (void)path;
    return -1;
}
int32_t PLUTO_SystemEventsHandlerDeregisterFileObserver(PLUTO_SystemEventHandler_t handler, int descriptor)
{
    (void)handler;
    (void)descriptor;
    return -1;
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
#else
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
    struct {
        int epoll_fd;
    } epoll;
    struct {
        int inotify_fd;
    } inotify;
    PLUTO_Logger_t logger;
};

PLUTO_SystemEventHandler_t PLUTO_CreateSystemEventHandler(PLUTO_Logger_t logger)
{
    assert(NULL != logger);

    PLUTO_SystemEventHandler_t handler = (PLUTO_SystemEventHandler_t)PLUTO_Malloc(sizeof(struct PLUTO_SystemEventHandler));
    handler->logger = logger;
    handler->epoll.epoll_fd = epoll_create1(0);
    if(handler->epoll.epoll_fd < 0)
    {
        PLUTO_Free(handler);
        return NULL;
    }
    handler->inotify.inotify_fd = inotify_init1(IN_NONBLOCK);
    if(handler->inotify.inotify_fd < 0)
    {
        close(handler->epoll.epoll_fd);
        PLUTO_Free(handler);
        return NULL;
    }
    
    struct epoll_event event = {
        .events = EPOLLIN | EPOLLOUT,
        .data.fd = handler->inotify.inotify_fd,
    };
    const int res = epoll_ctl(handler->epoll.epoll_fd, EPOLL_CTL_ADD, handler->inotify.inotify_fd, &event);
    if(res < 0)
    {
        close(handler->epoll.epoll_fd);
        close(handler->inotify.inotify_fd);
        PLUTO_Free(handler);
        return NULL;
    }
    return handler;
}

void PLUTO_DestroySystemEventHandler(PLUTO_SystemEventHandler_t *handler)
{
    assert(NULL != *handler);
    close((*handler)->epoll.epoll_fd);
    close((*handler)->inotify.inotify_fd);
    PLUTO_Free(*handler);
    *handler = NULL;
}

int32_t PLUTO_SystemEventHandlerRegisterFileObserver(PLUTO_SystemEventHandler_t handler, const char *path)
{
    assert(NULL != handler);
    assert(handler->inotify.inotify_fd >= 0);
    
    const int mask = IN_ATTRIB | IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MODIFY | IN_MOVE_SELF; 
    if(inotify_add_watch(handler->inotify.inotify_fd, path, mask) < 0)
    {
        PLUTO_LoggerWarning(handler->logger, "Unable to register Observer for File: %s, Error was: %s", path, strerror(errno));
        return PLUTO_SE_ERRROR;
    }
    return PLUTO_SE_OK;
}

int32_t PLUTO_SystemEventsHandlerDeregisterFileObserver(PLUTO_SystemEventHandler_t handler, int descriptor)
{
    assert(NULL != handler);
    assert(handler->inotify.inotify_fd >= 0);

    const int res = inotify_rm_watch(handler->inotify.inotify_fd, descriptor);
    if(res < 0)
    {
        if(EINVAL == errno)
        {
            return PLUTO_SE_OK;
        }
        PLUTO_LoggerWarning(
            handler->logger, 
            "Unable to deregister Observer for File: %i, Errno was: %i, Error was: %s", 
            descriptor, 
            errno,
            strerror(errno)
        );
        return PLUTO_SE_ERRROR;
    }
    return PLUTO_SE_OK;
}

int32_t PLUTO_SystemEventsHandlerRegisterObserver(PLUTO_SystemEventHandler_t handler, int descriptor)
{
    struct stat fstat_buffer;
    if(fstat(descriptor, &fstat_buffer) < 0)
    {
        PLUTO_LoggerWarning(handler->logger, "Unable to register Observer for Filedescriptor %i, Error was: %s", descriptor, strerror(errno));
        return PLUTO_SE_ERRROR;
    }
    if(S_ISREG(fstat_buffer.st_mode) || S_ISDIR(fstat_buffer.st_mode))
    {
        /*
        //
        // Work with inotify...
        //
        char file_path[4096];
        if(fcntl(descriptor, F_GETPATH, file_path) < 0)
        {
            PLUTO_LoggerWarning(handler->logger, "Unable to register Observer for Filedescriptor %i, Error was: %s", descriptor, strerror(errno));
            return PLUTO_SE_ERRROR;
        }
        if(inotify_add_watch(handler->inotify.inotify_fd, file_path, 0) < 0)
        {
            PLUTO_LoggerWarning(handler->logger, "Unable to register Observer for Filedescriptor %i, Error was: %s", descriptor, strerror(errno));
            return PLUTO_SE_ERRROR;
        }
        return PLUTO_SE_OK;
        */
        PLUTO_LoggerWarning(handler->logger, "Files can not be registered for System Events, use the Fileinterface instead.");
        return PLUTO_SE_ERRROR;
    }
    else if(S_ISFIFO(fstat_buffer.st_mode) || S_ISSOCK(fstat_buffer.st_mode))
    {
        //
        // Work with epoll...
        //
        const int res = epoll_ctl(handler->epoll.epoll_fd, EPOLL_CTL_ADD, descriptor, NULL);
        if(res < 0)
        {
            PLUTO_LoggerWarning(handler->logger, "Unable to register Observer for Filedescriptor %i, Error was: %s", descriptor, strerror(errno));
            return PLUTO_SE_ERRROR;
        }
        return PLUTO_SE_OK;
    }
    else
    {
        PLUTO_LoggerWarning(handler->logger, "Unable to register Observer for Filedescriptor %i, unknown Mode...", descriptor);
        return PLUTO_SE_ERRROR;
    }
}

int32_t PLUTO_SystemEventsHandlerDeregisterObserver(PLUTO_SystemEventHandler_t handler, int descriptor)
{
    const int res = epoll_ctl(handler->epoll.epoll_fd, EPOLL_CTL_DEL, descriptor, NULL);
    if(res < 0)
    {
        PLUTO_LoggerWarning(handler->logger, "Unable to deregister Observer for Filedescriptor %i, Error was: %s", descriptor, strerror(errno));
        return PLUTO_SE_ERRROR;
    }
    return PLUTO_SE_OK;
}

int32_t PLUTO_SystemEventsPoll(PLUTO_SystemEventHandler_t handler, PLUTO_SystemEvent_t event) 
{
    (void)event;
#define PLUTO_MAX_EPOLL_EVENTS_BUFFER 64
    struct epoll_event events[PLUTO_MAX_EPOLL_EVENTS_BUFFER];
    int res = epoll_wait(handler->epoll.epoll_fd, events, 1, 0);
    if(res < 0)
    {
        // error
        return PLUTO_SE_ERRROR;
    }

    if(res > 0)
    {
        printf("Epollin %i\n", EPOLLIN);
        printf("Event... 0x%x\n", events[0].events);
        return PLUTO_SE_OK;
    }
    return PLUTO_SE_NO_EVENT;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
#endif
