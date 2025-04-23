#ifndef __PLUTO_EDGE_H__
#define __PLUTO_EDGE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>


struct PLUTO_EDGE_Event;
typedef struct PLUTO_EDGE_Event* PLUTO_EDGE_Event_t;

struct PLUTO_EDGE_Queue;
typedef struct PLUTO_EDGE_Queue* PLUTO_EDGE_Queue_t;


PLUTO_EDGE_Event_t PLUTO_EDGE_CreateEvent(uint32_t id, uint32_t event, size_t size);
void PLUTO_EDGE_DestroyEvent(PLUTO_EDGE_Event_t event);
uint32_t PLUTO_EDGE_EventId(const PLUTO_EDGE_Event_t event);
uint32_t PLUTO_EDGE_EventEvent(const PLUTO_EDGE_Event_t event);
const char* PLUTO_EDGE_EventPayload(const PLUTO_EDGE_Event_t event);
time_t PLUTO_EDGE_EventTimestamp(const PLUTO_EDGE_Event_t event);


PLUTO_EDGE_Queue_t PLUTO_EDGE_CreateQueue(const char *name);
void PLUTO_EDGE_DestroyQueue(PLUTO_EDGE_Queue_t *queue);
bool PLUTO_EDGE_QueueWrite(
    PLUTO_EDGE_Queue_t queue, 
    const PLUTO_EDGE_Event_t event
);
bool PLUTO_EDGE_QueueRead(PLUTO_EDGE_Queue_t queue, PLUTO_EDGE_Event_t event);

#endif
