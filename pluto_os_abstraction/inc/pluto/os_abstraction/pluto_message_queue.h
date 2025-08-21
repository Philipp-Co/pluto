///
/// \brief  This Module contains the Interfacedefinition of a Messagequeue.
/// 
/// \requirements   1. The Queue must be usable for inter Process Communication.
///
#ifndef __PLUTO_MESSAGE_QUEUE_H__
#define __PLUTO_MESSAGE_QUEUE_H__


#if PLUTO_OS_INTERFACE == (PLUTO_OS_INTERFACE_SYSTEM_V)

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/config/pluto_compile_config.h>
#include <pluto/os_abstraction/pluto_types.h>
#include <pluto/os_abstraction/pluto_semaphore.h>
#include <pluto/os_abstraction/pluto_logger.h>

#include <stdbool.h>
#include <sys/msg.h>
#include <stdio.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_MAX_BODY_SIZE (256)

//
// --------------------------------------------------------------------------------------------------------------------
//

struct PLUTO_MessageQueue
{
    PLUTO_Key_t key __attribute__((aligned(64)));
    PLUTO_Semaphore_t semaphore;
    PLUTO_Logger_t logger;
    int filedescriptor;
} __attribute__((aligned(64)));
typedef struct PLUTO_MessageQueue* PLUTO_MessageQueue_t;

struct PLUTO_MsgBuf 
{
    long msgtype __attribute__((aligned(16)));
    char text[PLUTO_MAX_BODY_SIZE] __attribute__((aligned(16)));
} __attribute__((aligned(64)));

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Create a Message Queue.
///
PLUTO_MessageQueue_t PLUTO_CreateMessageQueue(
    const char *path, 
    const char *name, 
    unsigned int permissions,
    PLUTO_Logger_t logger
);
PLUTO_MessageQueue_t PLUTO_MessageQueueGet(const char *path, const char *name, PLUTO_Logger_t logger);
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
int32_t PLUTO_MessageQueueNumberOfMessagesAvailable(PLUTO_MessageQueue_t queue);

#endif 

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
