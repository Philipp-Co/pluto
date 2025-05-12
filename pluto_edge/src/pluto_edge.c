
#include "pluto/os_abstraction/pluto_message_queue.h"
#include "pluto/pluto_event/pluto_event.h"
#include <pluto/pluto_edge/pluto_edge.h>
#include <pluto/os_abstraction/pluto_malloc.h>


#include <assert.h>


#if PLUTO_LOGGER_ACTIVE
PLUTO_Logger_t PLUTO_edge_logger = NULL;
#endif

PLUTO_EDGE_Edge_t PLUTO_EDGE_CreateEdge(const char *path, const char *name, unsigned int permission)
{
#if PLUTO_LOGGER_ACTIVE
    if(!PLUTO_edge_logger)
    {
        PLUTO_edge_logger = PLUTO_CreateLogger("Edge");
    }
    PLUTO_LoggerInfo(
        PLUTO_edge_logger, 
        "Create Edge Object for Path %s, Name %s", 
        path,
        name
    );
#endif
    PLUTO_EDGE_Edge_t edge = PLUTO_Malloc(sizeof(struct PLUTO_EDGE_Edge));
    edge->queue = NULL;
#if PLUTO_LOGGER_ACTIVE
    edge->logger = NULL;
    char buffer[1024];
    const int result = snprintf(buffer, sizeof(buffer), "Edge=%s",name);
    if(result <= 0 || (size_t)result >= sizeof(buffer))
    {
        goto error;
    }
    edge->logger = PLUTO_CreateLogger(buffer); 
#endif
    edge->queue = PLUTO_CreateMessageQueue(
        path, name, permission
    );
    if(!edge->queue)
    {
        goto error;
    }
    return edge;
error:
    PLUTO_EDGE_DestroyEdge(&edge);
    return edge;
}

void PLUTO_EDGE_DestroyEdge(PLUTO_EDGE_Edge_t *edge)
{
    assert(NULL != *edge);
#if PLUTO_LOGGER_ACTIVE
    if((*edge)->logger)
        PLUTO_DestroyLogger(&(*edge)->logger);
#endif
    if((*edge)->queue)
        PLUTO_DestroyMessageQueue(&(*edge)->queue);
    PLUTO_Free(*edge);
    *edge = NULL;
}

bool PLUTO_EDGE_EdgeSendEvent(PLUTO_EDGE_Edge_t edge, const PLUTO_Event_t event)
{
    struct PLUTO_MsgBuf buffer;
    buffer.msgtype = 1;
    PLUTO_EventToBuffer(
        event,
        buffer.text,
        sizeof(buffer.text)
    );
    const bool result = PLUTO_MessageQueueWrite(
        edge->queue,
        &buffer
    );

#if PLUTO_LOGGER_ACTIVE
    if(!result)
    {
        PLUTO_LoggerWarning(
            edge->logger,
            "An Event could not be written to the Messagequeue."
        );
    }
#endif
    return result;
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
