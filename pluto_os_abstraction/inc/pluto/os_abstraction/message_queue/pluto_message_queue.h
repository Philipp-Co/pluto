///
/// \brief  This Module contains the Interfacedefinition of a Messagequeue.
///
/// \requirements   1. The Queue must be usable for inter Process Communication.
///
#ifndef __PLUTO_MESSAGE_QUEUE_H__
#define __PLUTO_MESSAGE_QUEUE_H__


//#if (PLUTO_OS_INTERFACE) == (PLUTO_OS_INTERFACE_SYSTEM_V)

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/config/pluto_compile_config.h>
#include <pluto/os_abstraction/pluto_types.h>
#include <pluto/os_abstraction/pluto_semaphore.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/message_queue/pluto_event.h>

#include <stdbool.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdatomic.h>

#ifdef PLUTO_MESSAGE_QUEUE_CUSTOM_0
#include <pluto/os_abstraction/pluto_shared_memory.h>
#include <stdatomic.h>
#endif

//
// --------------------------------------------------------------------------------------------------------------------
//
#define PLUTO_MAX_BODY_SIZE (sizeof(struct PLUTO_Event))

struct PLUTO_MsgBuf
{
    long msgtype; //__attribute__((aligned(16)));
    struct PLUTO_EventBuffer buffer;
};

struct PLUTO_MessageQueueInternal
{
    PLUTO_Key_t *key;
#ifdef PLUTO_MESSAGE_QUEUE_CUSTOM_0
    PLUTO_SharedMemory_t shared_memory;
#endif
    PLUTO_Semaphore_t semaphore;
    PLUTO_Logger_t logger;
};

#ifdef PLUTO_MESSAGE_QUEUE_CUSTOM_0
struct PLUTO_DataRow
{
    char data[64];
};

struct PLUTO_DataHeader
{
    atomic_uint write_lock;// __attribute__((aligned(64)));
    atomic_uint write_idx;// __attribute__((aligned(64)));
    atomic_uint read_idx; //__attribute__((aligned(64)));
} __attribute__((aligned(64)));

struct PLUTO_Data
{
    struct PLUTO_DataHeader header;
    struct PLUTO_DataRow data[64 * 31];
} __attribute__((aligned(4096)));
#endif

struct PLUTO_MessageQueue
{
    struct PLUTO_MessageQueueInternal *internal;
#ifdef PLUTO_MESSAGE_QUEUE_CUSTOM_0
    struct PLUTO_Data *data;
#endif
    int filedescriptor;
} __attribute__((aligned(64)));
typedef struct PLUTO_MessageQueue* PLUTO_MessageQueue_t;

typedef void* (*PLUTO_MessageQueueMalloc_t)(size_t size);
typedef void (*PLUTO_MessageQueueFree_t)(void *ptr);
struct PLUTO_MessageQueueAllocator
{
    PLUTO_MessageQueueMalloc_t malloc;
    PLUTO_MessageQueueFree_t free;
};

//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_MessageQueueSetAllocator(struct PLUTO_MessageQueueAllocator allocator);

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
bool PLUTO_MessageQueueRead(PLUTO_MessageQueue_t queue, PLUTO_Event_t event);
///
/// \brief  Write a Message to the Queue.
///         Append it at the End of the Queue.
///
bool PLUTO_MessageQueueWrite(PLUTO_MessageQueue_t queue, PLUTO_Event_t event);
int32_t PLUTO_MessageQueueNumberOfMessagesAvailable(PLUTO_MessageQueue_t queue);

//#endif

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
