#ifndef __PLUTO_MESSAGE_QUEUE_H__
#define __PLUTO_MESSAGE_QUEUE_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_types.h>
#include <pluto/pluto_semaphore.h>

#include <stdbool.h>
#include <sys/msg.h>
#include <stdio.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

struct PLUTO_MessageQueue
{
    PLUTO_Key_t key;
    PLUTO_Semaphore_t semaphore;
    int filedescriptor;
};
typedef struct PLUTO_MessageQueue* PLUTO_MessageQueue_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_MessageQueue_t PLUTO_CreateMessageQueue(const char *path, const char *name, unsigned int permissions);
void PLUTO_DestroyMessageQueue(PLUTO_MessageQueue_t *queue);
bool PLUTO_MessageQueueRead(PLUTO_MessageQueue_t queue, PLUTO_Request_t request);
bool PLUTO_MessageQueueWrite(PLUTO_MessageQueue_t queue, PLUTO_Response_t response);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
