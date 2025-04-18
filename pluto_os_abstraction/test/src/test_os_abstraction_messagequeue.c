#include <pluto/os_abstraction/test/test_os_abstraction_messagequeue.h>
#include <pluto/os_abstraction/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_logger.h>


void PLUTO_TEST_MessageQueueInitialCreate(void)
{
    PLUTO_Logger_t logger = PLUTO_CreateLogger("test");

    unsigned int permissions = 0777;
    const char *path = "/tmp/";
    const char *name = "testqueue";
    PLUTO_MessageQueue_t queue = PLUTO_CreateMessageQueue(
        path,
        name,
        permissions,
        logger
    );

    PLUTO_DestroyMessageQueue(&queue);
    PLUTO_DestroyLogger(&logger);
}

void PLUTO_TEST_MessageQueueInitialGet(void)
{
    PLUTO_Logger_t logger = PLUTO_CreateLogger("test");

    unsigned int permissions = 0777;
    const char *path = "/tmp/";
    const char *name = "testqueue";
    PLUTO_MessageQueue_t queue = PLUTO_CreateMessageQueue(
        path,
        name,
        permissions,
        logger
    );

    PLUTO_MessageQueue_t queue_get = PLUTO_MessageQueueGet(
        path,
        name,
        logger
    );

    PLUTO_DestroyMessageQueue(&queue_get);
    PLUTO_DestroyMessageQueue(&queue);
    PLUTO_DestroyLogger(&logger);
}
