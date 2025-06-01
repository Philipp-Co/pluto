#include "pluto/pluto_event/pluto_event.h"
#include <Unity/src/unity.h>
#include <pluto/pluto_edge/test/test_pluto_edge.h>
#include <pluto/pluto_edge/pluto_edge.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/pluto_message_queue.h>

#include <string.h>


void PLUTO_TEST_EdgeInitial(void)
{
    char *path = "/tmp/";
    char *name = "edge_test";
    unsigned int permission = 0777;
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(
        path,
        name,
        permission,
        PLUTO_CreateLogger("test-edge")
    );

    TEST_ASSERT_NOT_NULL(edge);

    PLUTO_EDGE_DestroyEdge(&edge);
}

void PLUTO_TEST_EdgeSendAndReceive(void)
{
    char *path = "/tmp/";
    char *name = "edge_test";
    unsigned int permission = 0777;
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(
        path,
        name,
        permission,
        PLUTO_CreateLogger("test-edge")
    );
    TEST_ASSERT_NOT_NULL(edge);


    PLUTO_Event_t event = PLUTO_CreateEvent();
    snprintf(
        PLUTO_EventPayload(event),
        PLUTO_EventSizeOfPayloadBuffer(event),
        "This is a Test!"
    );
    PLUTO_EventSetSizeOfPayload(event, strlen("This is a Test!"));
    PLUTO_EventSetId(event, 1);
    PLUTO_EventSetEvent(event, 42);
    PLUTO_EDGE_EdgeSendEvent(edge, event);

    PLUTO_Event_t recv_event = PLUTO_CreateEvent();
    TEST_ASSERT_TRUE(
        PLUTO_EDGE_EdgeReceiveEvent(edge, recv_event)
    );
    TEST_ASSERT_EQUAL_STRING(
        "This is a Test!",
        PLUTO_EventPayload(recv_event)
    );

    PLUTO_EDGE_DestroyEdge(&edge);
}

