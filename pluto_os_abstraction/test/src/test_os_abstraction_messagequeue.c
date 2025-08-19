#include <pluto/os_abstraction/test/test_os_abstraction_messagequeue.h>
#include <pluto/os_abstraction/pluto_message_queue.h>
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

    struct PLUTO_MsgBuf buffer;
    snprintf(
        buffer.text,
        sizeof(buffer.text),
        "test"
    );
    const bool result = PLUTO_MessageQueueWrite(queue, &buffer);
    TEST_ASSERT_TRUE(result);
    
    struct PLUTO_MsgBuf read_buffer;
    const bool read_result = PLUTO_MessageQueueRead(queue_get, &read_buffer);
    TEST_ASSERT_TRUE(read_result);
    TEST_ASSERT_EQUAL(0, strncmp(read_buffer.text, "test", strlen("test")));


    PLUTO_DestroyMessageQueue(&queue_get);
    PLUTO_DestroyMessageQueue(&queue);
    PLUTO_DestroyLogger(&logger);
}
