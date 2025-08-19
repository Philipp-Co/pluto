#include "pluto/pluto_event/pluto_event.h"
#include <Unity/src/unity.h>
#include <pluto/pluto_edge/test/test_pluto_edge.h>
#include <pluto/pluto_edge/pluto_edge.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/pluto_message_queue.h>

#include <string.h>


void PLUTO_TEST_EdgeInitial(void)
{
    printf("TEST 1\n");
    char *path = "/tmp/";
    char *name = "edge_test";
    unsigned int permission = 0777;
    printf("TEST 2\n");
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(
        path,
        name,
        permission,
        PLUTO_TEST_logger
    );
    printf("TEST 3\n");
    TEST_ASSERT_NOT_NULL(edge);
    printf("TEST 4\n");
    PLUTO_EDGE_DestroyEdge(&edge);
}

void PLUTO_TEST_EdgeSendAndReceive(void)
{
    char *path = "/tmp/";
    char *name = "edge_test";
    unsigned int permission = 0777;
    printf("TEST 1\n");
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(
        path,
        name,
        permission,
        PLUTO_TEST_logger
    );
    TEST_ASSERT_NOT_NULL(edge);

    printf("TEST 2\n");
    PLUTO_Event_t event = PLUTO_CreateEvent();
    snprintf(
        PLUTO_EventPayload(event),
        PLUTO_EventSizeOfPayloadBuffer(event),
        "This is a Test!"
    );
    PLUTO_EventSetSizeOfPayload(event, strlen("This is a Test!"));
    PLUTO_EventSetId(event, 1);
    PLUTO_EventSetEvent(event, 42);
    printf("TEST 3\n");
    PLUTO_EDGE_EdgeSendEvent(edge, event);

    printf("TEST 4\n");
    PLUTO_Event_t recv_event = PLUTO_CreateEvent();
    printf("TEST 5\n");
    TEST_ASSERT_TRUE(
        PLUTO_EDGE_EdgeReceiveEvent(edge, recv_event)
    );
    TEST_ASSERT_EQUAL_STRING(
        "This is a Test!",
        PLUTO_EventPayload(recv_event)
    );

    PLUTO_DestroyEvent(&event);
    PLUTO_DestroyEvent(&recv_event);
    PLUTO_EDGE_DestroyEdge(&edge);
}

