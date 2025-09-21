//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/message_queue/pluto_event.h>
#include <pluto/os_abstraction/pluto_malloc.h>
#include <pluto/os_abstraction/pluto_logger.h>

#define JSMN_HEADER
#include <jsmn/jsmn.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_PARSER_TOKEN_ID 1
#define PLUTO_PARSER_TOKEN_EVENT 2
#define PLUTO_PARSER_TOKEN_TIMESTAMP 4
#define PLUTO_PARSER_TOKEN_PAYLOAD 8

//
// --------------------------------------------------------------------------------------------------------------------
//

/*
static PLUTO_Logger_t PLUTO_event_logger = NULL;
*/

//
// --------------------------------------------------------------------------------------------------------------------
//


bool PLUTO_ReadTopLevelJSON(jsmntok_t *token, size_t size, const char *data, PLUTO_Event_t event);

PLUTO_Event_t PLUTO_CreateEvent(void)
{
    /*
    if(!PLUTO_event_logger)
    {
        PLUTO_event_logger = PLUTO_CreateLogger(
            "Event"
        );
    }
    */

    PLUTO_Time_t timestamp = {
        0
    };
    PLUTO_Event_t event = PLUTO_Malloc(sizeof(struct PLUTO_Event));
    event->header.id = 0;
    event->header.eventid = 0;
    event->header.timestamp = timestamp;
    event->header.nbytes_payload = 0LU;
    memset(event->payload, '\0', sizeof(event->payload));
    return event;
}

void PLUTO_DestroyEvent(PLUTO_Event_t *event)
{
    assert(NULL != event);
    assert(NULL != *event);
    PLUTO_Free(*event);
    *event = NULL;
}

bool PLUTO_CreateEventFromBuffer(PLUTO_Event_t event, const char *buffer, size_t nbytes)
{
    if(nbytes < sizeof(struct PLUTO_Event))
    {
        return false;
    }
    memcpy(event, buffer, sizeof(struct PLUTO_Event));
    /*
    (void)nbytes;
    assert(NULL != buffer);
    assert(NULL != event);

    jsmntok_t token[128];
    jsmn_parser parser;
    jsmn_init(&parser);

    event->header.nbytes_payload = 0;
    const int len = strlen(buffer);

    const int result = jsmn_parse(&parser, buffer, len, token, sizeof(token));
    if(result < 0)
    {
        return false;
    }

    if(!PLUTO_ReadTopLevelJSON(token, result, buffer, event))
    {
        return false;
    }

    if(event->header.nbytes_payload <= (sizeof(event->payload)-len))
    {
        memcpy(event->payload, buffer + len + 1, event->header.nbytes_payload);
        return true;
    }
    */
    return true;
}

size_t PLUTO_EventToBuffer(const PLUTO_Event_t event, char *buffer, uint16_t nbytes)
{
    if(nbytes < sizeof(struct PLUTO_Event))
    {
        return 0;
    }
    memcpy(buffer, (const void*)event, sizeof(struct PLUTO_Event));
    /*
    char timestamp[128];
    PLUTO_TimeToString(PLUTO_EventTimestamp(event), timestamp, sizeof(timestamp));
    const int result = snprintf(
        buffer,
        nbytes,
        "{\"id\":%i,\"event\":%i,\"time\":\"%s\",\"payload\":%lu}",
        event->header.id,
        event->header.eventid,
        timestamp,
        PLUTO_EventSizeOfPayload(event)
    );
    if((result > 0) && ((result + event->header.nbytes_payload) < nbytes))
    {
        memcpy(buffer + result + 1, event->payload, event->header.nbytes_payload);
        return true;
    }
    return false;
    */
    return sizeof(struct PLUTO_Event);
}

void PLUTO_EventSetTimestamp(PLUTO_Event_t event, PLUTO_Time_t timestamp)
{
    event->header.timestamp = timestamp;
}

void PLUTO_EventSetId(PLUTO_Event_t event, uint32_t id)
{
    event->header.id = id;
}

void PLUTO_EventSetEvent(PLUTO_Event_t event, uint32_t eventid)
{
    event->header.eventid = eventid;
}

uint32_t PLUTO_EventEventId(const PLUTO_Event_t event)
{
    return event->header.eventid;
}

uint32_t PLUTO_EventId(const PLUTO_Event_t event)
{
    return event->header.id;
}

PLUTO_Time_t PLUTO_EventTimestamp(const PLUTO_Event_t event)
{
    return event->header.timestamp;
}

char* PLUTO_EventPayload(PLUTO_Event_t event)
{
    return event->payload;
}

size_t PLUTO_EventSizeOfPayload(const PLUTO_Event_t event)
{
    return event->header.nbytes_payload;
}

size_t PLUTO_EventSizeOfPayloadBuffer(const PLUTO_Event_t event)
{
    return sizeof(event->payload);
}

void PLUTO_EventSetSizeOfPayload(PLUTO_Event_t event, uint16_t nbytes_payload)
{
    event->header.nbytes_payload = nbytes_payload > sizeof(event->payload) ? sizeof(event->payload) : nbytes_payload;
}

bool PLUTO_EventCopyBufferToPayload(PLUTO_Event_t event, const void *buffer, size_t nbytes)
{
    if(PLUTO_EventSizeOfPayloadBuffer(event) > nbytes)
    {
        memcpy(PLUTO_EventPayload(event), buffer, nbytes);
        PLUTO_EventSetSizeOfPayload(event, nbytes);
        return true;
    }
    return false;
}

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Try to find out to which key "key" Points.
/// \returns One of PLUTO_PARSER_TOKEN_* Values on Success or 0 on Error.
///
static unsigned int PLUTO_ReadKey(const jsmntok_t *key, const char *data);

bool PLUTO_ReadTopLevelJSON(jsmntok_t *token, size_t size, const char *data, PLUTO_Event_t event)
{
    assert(NULL != token);
    assert(size > 0);
    assert(NULL != data);
    assert(NULL != event);

    event->header.id = 0U;
    memset(event->payload, '\0', sizeof(event->payload));

    if(JSMN_OBJECT != token[0].type)
    {
        return false;
    }

    char buffer[1024];
    unsigned int result = 0U;
    char *tmp;
    for(size_t i=1U;i<size;)
    {
        const unsigned int obj = PLUTO_ReadKey(&token[i], data);
        if((i+1) >= size)
        {
            return false;
        }
        switch(obj)
        {
            case PLUTO_PARSER_TOKEN_ID:
                memcpy(buffer, data + token[i+1].start, token[i+1].end - token[i+1].start);
                buffer[token[i+1].end - token[i+1].start] = '\0';
                if('-' == buffer[0]) return false;
                event->header.id = (uint32_t)strtoul(buffer, &tmp, 10);
                if(buffer == tmp) return false;
                i += 2;
                break;
            case PLUTO_PARSER_TOKEN_EVENT:
                memcpy(buffer, data + token[i+1].start, token[i+1].end - token[i+1].start);
                buffer[token[i+1].end - token[i+1].start] = '\0';
                if('-' == buffer[0]) return false;
                event->header.eventid = (uint32_t)strtoul(buffer, &tmp, 10);
                if(buffer == tmp) return false;
                i += 2;
                break;
            case PLUTO_PARSER_TOKEN_TIMESTAMP:
                event->header.timestamp = PLUTO_TimeFromString(buffer);
                i += 2;
                break;
            case PLUTO_PARSER_TOKEN_PAYLOAD:
                memcpy(buffer, data + token[i+1].start, token[i+1].end - token[i+1].start);
                buffer[token[i+1].end - token[i+1].start] = '\0';
                if('-' == buffer[0]) return false;
                event->header.nbytes_payload = (uint32_t)strtoul(buffer, &tmp, 10);
                if(buffer == tmp) return false;
                i += 2;
                break;
            default:
                return false;
        }
        result |= obj;
    }

    return (
        PLUTO_PARSER_TOKEN_ID |
        PLUTO_PARSER_TOKEN_EVENT |
        PLUTO_PARSER_TOKEN_TIMESTAMP |
        PLUTO_PARSER_TOKEN_PAYLOAD
    ) == result;
}

static unsigned int PLUTO_ReadKey(const jsmntok_t *key, const char *data)
{
    (void)data;
    char key_buffer[128];

    const size_t strl = key->end - key->start;
    memcpy(key_buffer, data + key->start, strl);
    key_buffer[strl] = '\0';

    #define PLUTO_PARSER_N_KEYS 4
    static const char *expected_keys[PLUTO_PARSER_N_KEYS] =
    {
        "id",
        "event",
        "time",
        "payload"
    };
    static const unsigned int expected_strl[PLUTO_PARSER_N_KEYS] =
    {
        2U,
        5U,
        4U,
        7U
    };
    static const unsigned int result_values[PLUTO_PARSER_N_KEYS] =
    {
        PLUTO_PARSER_TOKEN_ID,
        PLUTO_PARSER_TOKEN_EVENT,
        PLUTO_PARSER_TOKEN_TIMESTAMP,
        PLUTO_PARSER_TOKEN_PAYLOAD
    };

    for(size_t i=0U;i<PLUTO_PARSER_N_KEYS;++i)
    {
        if(strl == expected_strl[i])
        {
            if(0 == memcmp(expected_keys[i], key_buffer, strl))
            {
                return result_values[i];
            }
        }
    }
    return 0U;
}

//
// --------------------------------------------------------------------------------------------------------------------
//
