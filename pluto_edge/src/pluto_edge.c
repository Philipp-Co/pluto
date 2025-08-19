
#include "pluto/os_abstraction/pluto_message_queue.h"
#include "pluto/pluto_event/pluto_event.h"
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

bool PLUTO_EDGE_EdgeSendEvent(PLUTO_EDGE_Edge_t edge, const PLUTO_Event_t event)
{
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

    bool result = PLUTO_MessageQueueWrite(
        edge->queue,
        &buffer
    );
    if(!result)
    {
        PLUTO_LoggerWarning(
            edge->logger,
            "An Event could not be written to the Messagequeue."
        );
    }
    else
    {
        return true;
    }
    return false;
}

bool PLUTO_EDGE_EdgeReceiveEvent(PLUTO_EDGE_Edge_t edge, PLUTO_Event_t event)
{
    struct PLUTO_MsgBuf buffer;
    if(PLUTO_MessageQueueRead(edge->queue, &buffer))
    {
        return PLUTO_CreateEventFromBuffer(
            event,
            buffer.text,
            sizeof(buffer.text)
        );
    }
    return false;
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
