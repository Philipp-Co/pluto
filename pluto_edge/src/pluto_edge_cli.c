#include <stdio.h>
#include <string.h>
#include <pluto/pluto_edge/pluto_edge.h>
#include <pluto/os_abstraction/pluto_logger.h>


int main(int argc, char **argv)
{
    //
    //                1    2          3   4
    // pluto_edge_cli path queue_name r/w message
    //
    PLUTO_Logger_t logger = PLUTO_CreateLogger("Root");
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(
        argv[1], argv[2], 0, logger
    );
    PLUTO_Event_t event = PLUTO_CreateEvent();
    int return_value = -1;
    if(4 == argc && argv[3][0] == 'r')
    {
        if(!PLUTO_EDGE_EdgeReceiveEvent(edge, event))
        {
            PLUTO_LoggerError(logger, "Unable to receive an Event from the given Queue...");
            return_value = -1;
        }
        else
        {
            char buffer[sizeof(event->payload) + 1];
            memcpy(buffer, event->payload, event->header.nbytes_payload);
            PLUTO_LoggerInfo(logger, "Read Event-ID: %u, Id: %u, Payload: %s from Queue.", event->header.eventid, event->header.id, buffer);
            return_value = 0;
        }
    }
    else if(5 == argc && argv[3][0] == 'w')
    {
        event->header.eventid = 0U;
        event->header.id = 0U;
        event->header.nbytes_payload = strlen(argv[4]);
        event->header.timestamp = PLUTO_TimeNow();
        memset(event->payload, '\0', sizeof(event->payload));
        memcpy(event->payload, argv[4], strlen(argv[4]));
        if(!PLUTO_EDGE_EdgeSendEvent(edge, event))
        {
            PLUTO_LoggerError(logger, "Unable to send an Event to the given Queue...");
            return_value = -1;
        }
        else
        {
            PLUTO_LoggerInfo(logger, "Send %s to Queue %s/%s.", argv[4], argv[1], argv[2]);
            return_value = 0;
        }
    }
    PLUTO_EDGE_DestroyEdge(&edge);
    return return_value;
}

