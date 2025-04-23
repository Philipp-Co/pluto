
#include <pluto/pluto_edge/pluto_edge.h>

#include <stdlib.h>
#include <sys/msg.h>
#include <stdio.h>


struct PLUTO_EDGE_Event
{
    uint32_t id;
    uint32_t event;
    time_t timestamp;
    int32_t max_bytes_payload;
    char *payload;
};

struct PLUTO_EDGE_Queue
{
    int filedescriptor;
};

uint32_t PLUTO_EDGE_EventId(const PLUTO_EDGE_Event_t event)
{
    return event->id;
}

uint32_t PLUTO_EDGE_EventEvent(const PLUTO_EDGE_Event_t event)
{
    return event->event;
}

char* PLUTO_EDGE_EventPayload(const PLUTO_EDGE_Event_t event)
{
    return event->payload;
}  

size_t PLUTO_EDGE_EventPayloadBufferSize(const PLUTO_EDGE_Event_t event)
{
    return event->max_bytes_payload;
}

time_t PLUTO_EDGE_EventTimestamp(const PLUTO_EDGE_Event_t event)
{
    return event->timestamp;
}

PLUTO_EDGE_Event_t PLUTO_EDGE_CreateEvent(uint32_t id, uint32_t event, size_t size)
{
    PLUTO_EDGE_Event_t event_object = malloc(sizeof(struct PLUTO_EDGE_Event));
    event_object->id = id;
    event_object->event = event;
    event_object->payload = malloc(size);
    event_object->max_bytes_payload = size;
    event_object->timestamp = time(NULL);
    return event_object;
}

void PLUTO_EDGE_DestroyEvent(PLUTO_EDGE_Event_t event)
{
    event->id = 0;
    event->event = 0;
    event->max_bytes_payload = 0;
    if(event->payload)
    {
        free(event->payload);
    }
    event->payload = NULL;
    event->timestamp = 0;
}

PLUTO_EDGE_Queue_t PLUTO_EDGE_CreateQueue(const char *name)
{
    (void)name;
    PLUTO_EDGE_Queue_t queue = malloc(sizeof(struct PLUTO_EDGE_Queue));

    key_t key = ftok(name, 1);
    queue->filedescriptor = msgget(key, 0);
    if(queue->filedescriptor < 0)
    {
        free(queue);
        return NULL;
    }

    return queue;
}

void PLUTO_EDGE_DestroyQueue(PLUTO_EDGE_Queue_t *queue)
{
    free(*queue);
}

bool PLUTO_EDGE_QueueWrite(
    PLUTO_EDGE_Queue_t queue, 
    const PLUTO_EDGE_Event_t event
)
{
    char buffer[4096];
    
   time_t loctime = time(NULL); 
    struct tm *tm = localtime(&loctime);
    char time_buffer[1024];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%dT%H:%M:%S.%f%z", tm);

    snprintf(
        buffer,
        sizeof(buffer),
        "{\"id\":%i,\"event\":\"%i\",\"time\":\"%s\",\"payload\":\"%s\"}",
        event->id,
        event->event,
        time_buffer,
        event->payload
    );
    const int result = msgsnd(
        queue->filedescriptor,
        buffer,
        sizeof(buffer),
        IPC_NOWAIT
    );
    return result > 0;
}

static bool PLUTO_EDGE_ParseEvent(const char *message, PLUTO_EDGE_Event_t event);

bool PLUTO_EDGE_QueueRead(
    PLUTO_EDGE_Queue_t queue, 
    PLUTO_EDGE_Event_t event
)
{
    char buffer[4096];
    const int result = msgrcv(
        queue->filedescriptor,
        buffer,
        sizeof(buffer),
        IPC_NOWAIT,
        0
    ); 
    if(result > 0)
    {
        return PLUTO_EDGE_ParseEvent(buffer, event);
    }
    return false;
}

static bool PLUTO_EDGE_ParseEvent(const char *message, PLUTO_EDGE_Event_t event)
{
    (void)message;
    (void)event;
    return false;
}
