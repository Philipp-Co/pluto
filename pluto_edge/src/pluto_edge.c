
#include <pluto/os_abstraction/message_queue/pluto_event.h>
#include <pluto/pluto_edge/pluto_edge.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <assert.h>

struct PLUTO_EDGE_Edge
{
    PLUTO_MessageQueue_t queue;
    PLUTO_Logger_t logger;
};

PLUTO_EDGE_Edge_t PLUTO_EDGE_CreateEdge(
    const char *path, const char *name, unsigned int permission, PLUTO_Logger_t logger
)
{
    (void)permission;
    PLUTO_LoggerInfo(
        logger,
        "Create Edge Object for Path %s, Name %s",
        path,
        name
    );
    PLUTO_EDGE_Edge_t edge = PLUTO_Malloc(sizeof(struct PLUTO_EDGE_Edge));
    edge->queue = NULL;
    edge->logger = logger;
    char buffer[1024];
    const int result = snprintf(buffer, sizeof(buffer), "Edge=%s",name);
    if(result <= 0 || (size_t)result >= sizeof(buffer))
    {
        PLUTO_Free(edge);
        return NULL;
    }
    edge->queue = PLUTO_MessageQueueGet(
        path, name, logger
    );
    if(!edge->queue)
    {
        PLUTO_Free(edge);
        PLUTO_LoggerWarning(logger, "Error, unable to create Edge!");
        return NULL;
    }
    return edge;
}

void PLUTO_EDGE_DestroyEdge(PLUTO_EDGE_Edge_t *edge)
{
    assert(NULL != *edge);
    assert(NULL != (*edge)->queue);
    PLUTO_DestroyMessageQueue(&(*edge)->queue);
    PLUTO_Free(*edge);
    *edge = NULL;
}

PLUTO_Event_t PLUTO_EDGE_CreateEvent(void)
{
    return PLUTO_CreateEvent();
}

void PLUTO_EDGE_EventSetId(PLUTO_Event_t event, uint32_t id)
{
    event->header.id = id;
}

void PLUTO_EDGE_EventSetEventId(PLUTO_Event_t event, uint32_t id)
{
    event->header.eventid = id;
}

struct PLUTO_EDGE_Timestamp PLUTO_EDGE_EventsTimestamp(PLUTO_Event_t event)
{
    struct PLUTO_EDGE_Timestamp ts = {
        .year=PLUTO_TimeYear(event->header.timestamp),
        .month=PLUTO_TimeMonth(event->header.timestamp),
        .day=PLUTO_TimeDay(event->header.timestamp),
        .hour=PLUTO_TimeHour(event->header.timestamp),
        .minutes=PLUTO_TimeMinutes(event->header.timestamp),
        .seconds=PLUTO_TimeSeconds(event->header.timestamp),
        .milliseconds=PLUTO_TimeMilliseconds(event->header.timestamp)
    };
    return ts;
} 

void PLUTO_EDGE_DestroyEvent(PLUTO_Event_t *event)
{
    PLUTO_DestroyEvent(event);
}

bool PLUTO_EDGE_EdgeSendEvent(PLUTO_EDGE_Edge_t edge, const PLUTO_Event_t event)
{
    /*
    struct PLUTO_MsgBuf buffer;
    buffer.msgtype = 1;
    if(
        !PLUTO_EventToBuffer(
            event,
            buffer.text,
            sizeof(buffer.text)
        )
    )
    {
        return false;
    }
    */
    printf("Edge addr: %p\n", (void*)edge);
    printf("Queue addr: %p\n", (void*)edge->queue);
    const bool result = PLUTO_MessageQueueWrite(
        edge->queue,
        //&buffer
        event
    );
    if(!result)
    {
        PLUTO_LoggerWarning(
            edge->logger,
            "An Event could not be written to the Messagequeue."
        );
        return false;
    }
    else
    {
        return true;
    }
}

bool PLUTO_EDGE_EdgeReceiveEvent(PLUTO_EDGE_Edge_t edge, PLUTO_Event_t event)
{
    return PLUTO_MessageQueueRead(edge->queue, event);
}

int32_t PLUTO_EDGE_NumberOfMessagesAvailable(PLUTO_EDGE_Edge_t edge)
{
    return PLUTO_MessageQueueNumberOfMessagesAvailable(
        edge->queue
    );
}

const char* PLUTO_EDGE_EdgeVersion(void)
{
#if !defined(PLUTO_EDGE_VERSION)
    return "0.0.0";
#else
    return PLUTO_EDGE_VERSION;
#endif
}
