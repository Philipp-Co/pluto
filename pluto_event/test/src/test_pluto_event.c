//
// --------------------------------------------------------------------------------------------------------------------
//

#include "Unity/src/unity_internals.h"
#include <pluto/pluto_event/test/test_pluto_event.h>
#include <pluto/pluto_event/pluto_event.h>

#include <Unity/src/unity.h>
#include <string.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_TEST_TestEvent(void)
{
    PLUTO_Event_t event = PLUTO_CreateEvent();

    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_EQUAL_UINT32(0U, PLUTO_EventId(event));
    TEST_ASSERT_EQUAL_UINT32(0U, PLUTO_EventEventId(event));
    TEST_ASSERT_EQUAL_UINT32((uint32_t)0U, (uint32_t)PLUTO_EventSizeOfPayload(event));
    TEST_ASSERT_NOT_NULL(PLUTO_EventPayload(event));

    PLUTO_DestroyEvent(&event);
    TEST_ASSERT_NULL(event);
}

//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_TEST_EventFromBufferSuccess(void)
{
    static const char *event_buffer = "{\"id\":4,\"event\":5,"
        "\"time\":\"2025-01-01T00:00:00.0+0000\","
        "\"payload\":6}\0"
        "\"test\"";
    PLUTO_Event_t event = PLUTO_CreateEvent();
    const bool result = PLUTO_CreateEventFromBuffer(event, event_buffer, strlen(event_buffer));

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(
        6,
        PLUTO_EventSizeOfPayload(event)
    );
    TEST_ASSERT_EQUAL_INT(
        0, 
        memcmp("\"test\"", PLUTO_EventPayload(event), strlen(PLUTO_EventPayload(event)))
    );
    TEST_ASSERT_EQUAL_UINT32(
        4U,
        PLUTO_EventId(event)
    );
    TEST_ASSERT_EQUAL_UINT32(
        5U,
        PLUTO_EventEventId(event)
    );
    PLUTO_DestroyEvent(&event);
}
//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_TEST_EventFromBufferIdMissing(void)
{
    static const char *event_buffer[3] = {
        "{\"event\":5,"
        "\"time\":\"2025-01-01T00:00:00.0+0000\","
        "\"payload\":6}\0"
        "\"test\"",
        "{\"id\", \"event\":5,"
        "\"time\":\"2025-01-01T00:00:00.0+0000\","
        "\"payload\":6}\0"
        "\"test\"",
        "{\"id\":-1, \"event\":5,"
        "\"time\":\"2025-01-01T00:00:00.0+0000\","
        "\"payload\":6}\0"
        "\"test\"",
    };
    PLUTO_Event_t event = PLUTO_CreateEvent();
    for(int32_t i=0;i<3;++i)
    {
        TEST_ASSERT_FALSE(
            PLUTO_CreateEventFromBuffer(event, event_buffer[i], strlen(event_buffer[i]))
        );
    }
    PLUTO_DestroyEvent(&event);
}
//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_TEST_EventFromBufferEventMissing(void)
{
    static const char *event_buffer[3] = { 
        "{\"id\":4,"
        "\"time\":\"2025-01-01T00:00:00.0+0000\","
        "\"payload\":6}\0"
        "\"test\"",
        "{\"id\":4,\"event\":-1,"
        "\"time\":\"2025-01-01T00:00:00.0+0000\","
        "\"payload\":6}\0"
        "\"test\"",
        "{\"id\":4,\"event\""
        "\"time\":\"2025-01-01T00:00:00.0+0000\","
        "\"payload\":6}\0"
        "\"test\""
    };
    PLUTO_Event_t event = PLUTO_CreateEvent();
    for(int32_t i=0;i<3;++i)
    {
        TEST_ASSERT_FALSE(
            PLUTO_CreateEventFromBuffer(event, event_buffer[i], strlen(event_buffer[i]))
        );
    }
    PLUTO_DestroyEvent(&event);
}
//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_TEST_EventFromBufferPayloadMissing(void)
{
    static const char *event_buffer[3] = {
        "{\"id\":4,\"event\":5,"
            "\"time\":\"2025-01-01T00:00:00.0+0000\"}\0"
            "\"test\"",
        "{\"id\":4,\"event\":5,"
            "\"time\":\"2025-01-01T00:00:00.0+0000\",\"payload\"}\0"
            "\"test\"",
        "{\"id\":4,\"event\":5,"
            "\"time\":\"2025-01-01T00:00:00.0+0000\",\"payload\":-1}\0"
            "\"test\""
    };
    PLUTO_Event_t event = PLUTO_CreateEvent();
    for(int32_t i=0;i<3;++i)
    {
        TEST_ASSERT_FALSE(
            PLUTO_CreateEventFromBuffer(event, event_buffer[i], strlen(event_buffer[i]))
        );
    }
    PLUTO_DestroyEvent(&event);
}

//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_TEST_EventToString(void)
{
    PLUTO_Event_t event = PLUTO_CreateEvent();
    PLUTO_EventSetId(event, 2);
    PLUTO_EventSetEvent(event, 3);
    const int len = snprintf(PLUTO_EventPayload(event), PLUTO_EventSizeOfPayloadBuffer(event), "test");
    PLUTO_EventSetSizeOfPayload(event, len);

    char buffer[4096];
    const bool result = PLUTO_EventToBuffer(event, buffer, sizeof(buffer));
    TEST_ASSERT_TRUE(result);
    static const char *expected = "{\"id\":2,"
        "\"event\":3,"
        "\"time\":\"1900-01-00T00:00:00.0\","
        "\"payload\":4"
        "}";
    
    TEST_ASSERT_EQUAL_INT(
        strlen("test"),
        PLUTO_EventSizeOfPayload(event)
    );
    TEST_ASSERT_EQUAL_STRING(
        expected,
        buffer
    );
    TEST_ASSERT_EQUAL_MEMORY(
        "test",
        buffer + strlen(expected) + 1,
        PLUTO_EventSizeOfPayload(event)
    );
}

//
// --------------------------------------------------------------------------------------------------------------------
//
