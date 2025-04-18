#include <pluto/os_abstraction/pluto_message_queue.h>

//
// Only Build this if this Flag is set.
//
#ifdef PLUTO_MESSAGE_QUEUE_CUSTOM_0

PLUTO_MessageQueue_t PLUTO_CreateMessageQueue(
    const char *path, 
    const char *name, 
    unsigned int permissions,
    PLUTO_Logger_t logger
)
{
    (void)path;
    (void)name;
    (void)permissions;
    (void)logger;
    return NULL;
}

PLUTO_MessageQueue_t PLUTO_MessageQueueGet(const char *path, const char *name, PLUTO_Logger_t logger)
{
    (void)path;
    (void)name;
    (void)logger;
    return NULL;
}
void PLUTO_DestroyMessageQueue(PLUTO_MessageQueue_t *queue)
{
    (void)queue;
}
bool PLUTO_MessageQueueRead(PLUTO_MessageQueue_t queue, struct PLUTO_MsgBuf *buffer)
{
    (void)queue;
    (void)buffer;
    return false;
}
bool PLUTO_MessageQueueWrite(PLUTO_MessageQueue_t queue, struct PLUTO_MsgBuf *buffer)
{
    (void)queue;
    (void)buffer;
    return false;
}
int32_t PLUTO_MessageQueueNumberOfMessagesAvailable(PLUTO_MessageQueue_t queue)
{
    (void)queue;
    return 0;
}

#endif
