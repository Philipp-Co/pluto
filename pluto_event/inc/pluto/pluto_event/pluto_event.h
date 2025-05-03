#ifndef __PLUTO_EVENT_H__
#define __PLUTO_EVENT_H__

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>


struct PLUTO_Event 
{
    uint32_t id;
    uint32_t eventid;
    time_t timestamp;
    size_t nbytes_payload;
    char *payload;
};
typedef struct PLUTO_Event* PLUTO_Event_t;


PLUTO_Event_t PLUTO_CreateEvent(size_t nbytes_payload);
void PLUTO_DestroyEvent(PLUTO_Event_t *event);
PLUTO_Event_t PLUTO_CreateEvetFromString(const char *str);
bool PLUTO_EventToString(const PLUTO_Event_t event, char *buffer, size_t nbytes);

void PLUTO_EventSetId(PLUTO_Event_t event, uint32_t id);
void PLUTO_EventSetEvent(PLUTO_Event_t event, uint32_t eventid);

uint32_t PLUTO_EventEventId(const PLUTO_Event_t event);
uint32_t PLUTO_EventId(const PLUTO_Event_t event);
time_t PLUTO_EventTimestamp(const PLUTO_Event_t event);
char* PLUTO_EventPayload(PLUTO_Event_t event);
size_t PLUTO_EventSizeOfPayload(const PLUTO_Event_t event);


#endif
