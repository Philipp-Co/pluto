///
/// \brief  Implementation of a IPC Queue.
///
#ifndef __PLUTO_MESSAGE_QUEUE_H__
#define __PLUTO_MESSAGE_QUEUE_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/pluto_types.h>
#include <pluto/os_abstraction/pluto_semaphore.h>

#include <stdbool.h>
#include <sys/msg.h>
#include <stdio.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_MAX_BODY_SIZE (4096)

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

struct PLUTO_MsgBuf 
{
    long msgtype;
    char text[PLUTO_MAX_BODY_SIZE];
};

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Create a Message Queue.
///
PLUTO_MessageQueue_t PLUTO_CreateMessageQueue(const char *path, const char *name, unsigned int permissions);
///
/// \brief  Destroy a Queue.
///
void PLUTO_DestroyMessageQueue(PLUTO_MessageQueue_t *queue);
///
/// \brief  Read a Message from the Queue.
///         Remove the oldest Element from the Queue and return it.
///
bool PLUTO_MessageQueueRead(PLUTO_MessageQueue_t queue, struct PLUTO_MsgBuf *buffer);
///
/// \brief  Write a Message to the Queue.
///         Append it at the End of the Queue.
///
bool PLUTO_MessageQueueWrite(PLUTO_MessageQueue_t queue, struct PLUTO_MsgBuf *buffer);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
