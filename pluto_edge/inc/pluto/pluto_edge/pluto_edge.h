#ifndef __PLUTO_EDGE_H__
#define __PLUTO_EDGE_H__

#include <pluto/os_abstraction/message_queue/pluto_event.h>
#include <pluto/os_abstraction/files/pluto_file.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/message_queue/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_logger.h>

#include <stdbool.h>

struct PLUTO_EDGE_Timestamp
{
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minutes;
    uint32_t seconds;
    uint32_t milliseconds;
};

struct PLUTO_EDGE_Edge;
typedef struct PLUTO_EDGE_Edge* PLUTO_EDGE_Edge_t;

PLUTO_EDGE_Edge_t PLUTO_EDGE_CreateEdge(
    const char *path,
    const char *name,
    unsigned int permission,
    PLUTO_Logger_t logger
);
void PLUTO_EDGE_DestroyEdge(PLUTO_EDGE_Edge_t *edge);

bool PLUTO_EDGE_EdgeSendEvent(PLUTO_EDGE_Edge_t edge, const PLUTO_Event_t event);
bool PLUTO_EDGE_EdgeReceiveEvent(PLUTO_EDGE_Edge_t edge, PLUTO_Event_t event);
int32_t PLUTO_EDGE_NumberOfMessagesAvailable(PLUTO_EDGE_Edge_t edge);
const char* PLUTO_EDGE_EdgeVersion(void);

PLUTO_Event_t PLUTO_EDGE_CreateEvent(void);
void PLUTO_EDGE_EventSetId(PLUTO_Event_t event, uint32_t id);
void PLUTO_EDGE_EventSetEventId(PLUTO_Event_t event, uint32_t id);
struct PLUTO_EDGE_Timestamp PLUTO_EDGE_EventsTimestamp(PLUTO_Event_t event); 
void PLUTO_EDGE_DestroyEvent(PLUTO_Event_t *event);

#endif
