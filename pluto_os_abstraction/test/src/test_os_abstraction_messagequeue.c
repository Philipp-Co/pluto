#include "pluto/os_abstraction/message_queue/pluto_event.h"
#include <pluto/os_abstraction/test/test_os_abstraction_messagequeue.h>
#include <pluto/os_abstraction/message_queue/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_logger.h>

#include <Unity/src/unity.h>
#include <string.h>

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

void PLUTO_TEST_MessageQueueSendAndRead(void)
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

    PLUTO_Event_t event = PLUTO_CreateEvent();
    snprintf(
        PLUTO_EventPayload(event),
        PLUTO_EventSizeOfPayloadBuffer(event),
        "test"
    );
    PLUTO_EventSetSizeOfPayload(event, strlen("test"));
    const bool result = PLUTO_MessageQueueWrite(queue, event);
    TEST_ASSERT_TRUE(result);

    PLUTO_Event_t received_event = PLUTO_CreateEvent();
    const bool read_result = PLUTO_MessageQueueRead(queue_get, received_event);
    TEST_ASSERT_TRUE(read_result);
    TEST_ASSERT_EQUAL(0, strncmp(PLUTO_EventPayload(received_event), "test", strlen("test")));


    PLUTO_DestroyMessageQueue(&queue_get);
    PLUTO_DestroyMessageQueue(&queue);
    PLUTO_DestroyLogger(&logger);
}
