#ifndef __PLUTO_RW_MESSAGE_QUEUE_H__
#define __PLUTO_RW_MESSAGE_QUEUE_H__

#include <stdbool.h>
#include <stddef.h>


struct PLUTO_RW_MessageBuffer
{
    long msgtype;
    char text[1024 * 64];
};
typedef struct PLUTO_RW_MessageBuffer* PLUTO_RW_MessageBuffer_t;

struct PLUTO_RW_Message
{
    size_t size;
    struct PLUTO_RW_MessageBuffer buffer;
};
typedef struct PLUTO_RW_Message* PLUTO_RW_Message_t;

struct PLUTO_RW_MessageQueue
{
    int queue_handle;
};
typedef struct PLUTO_RW_MessageQueue* PLUTO_RW_MessageQueue_t;

PLUTO_RW_MessageQueue_t PLUTO_RW_CreateMessageQueue(const char *name);
void PLUTO_RW_DestroyMessageQueue(PLUTO_RW_MessageQueue_t *queue);

bool PLUTO_RW_MessageQueueWrite(PLUTO_RW_MessageQueue_t queue, const PLUTO_RW_Message_t msg); 
bool PLUTO_RW_MessageQueueRead(PLUTO_RW_MessageQueue_t queue, PLUTO_RW_Message_t msg);

#endif
