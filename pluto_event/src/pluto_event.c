
#include <pluto/pluto_event/pluto_event.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <jsmn/jsmn.h>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


#define max(x, y)((x) < (y) ? (y) : (x))

#define PLUTO_PARSER_TOKEN_ID 1
#define PLUTO_PARSER_TOKEN_EVENT 2
#define PLUTO_PARSER_TOKEN_TIMESTAMP 4
#define PARSER_TOKEN_PAYLOAD 8



bool PLUTO_ReadTopLevelJSON(jsmntok_t *token, size_t size, const char *data, PLUTO_Event_t event);

PLUTO_Event_t PLUTO_CreateEvent(size_t nbytes_payload)
{
    PLUTO_Event_t event = PLUTO_Malloc(sizeof(struct PLUTO_Event));
    event->id = 0;
    event->eventid = 0;
    event->timestamp = 0;
    event->nbytes_payload = nbytes_payload;
    event->payload = PLUTO_Malloc(nbytes_payload);
    return event;
}

void PLUTO_DestroyEvent(PLUTO_Event_t *event)
{
    assert(NULL != *event);
    PLUTO_Free(*event);
    *event = NULL;
}

PLUTO_Event_t PLUTO_CreateEvetFromString(const char *str)
{
    jsmntok_t token[128];
    jsmn_parser parser;
    jsmn_init(&parser);
    
    const int result = jsmn_parse(&parser, str, strlen(str), token, sizeof(token));
    if(result < 0)
    {
        return NULL;
    }
    
    PLUTO_Event_t event = PLUTO_CreateEvent(4096);

    if(!PLUTO_ReadTopLevelJSON(token, result, str, event))
    {
        PLUTO_DestroyEvent(&event);
        return NULL;
    }
    return event;
}

bool PLUTO_EventToString(const PLUTO_Event_t event, char *buffer, size_t nbytes)
{
    char timestamp[1024];
    snprintf(timestamp, sizeof(timestamp), "Y-m-dTH:M:S.sz");
    const int result = snprintf(
        buffer, 
        nbytes, 
        "{\"id\":%i,\"event\":%i,\"timestamp\":\"%s\",\"payload\":\"%s\"}",
        event->id,
        event->eventid,
        timestamp,
        event->payload
    );
    return result > 0;
}

void PLUTO_EventSetId(PLUTO_Event_t event, uint32_t id)
{
    event->id = id;
}

void PLUTO_EventSetEvent(PLUTO_Event_t event, uint32_t eventid)
{
    event->eventid = eventid;
}

uint32_t PLUTO_EventEventId(const PLUTO_Event_t event)
{
    return event->eventid;
}

uint32_t PLUTO_EventId(const PLUTO_Event_t event)
{
    return event->id;
}

time_t PLUTO_EventTimestamp(const PLUTO_Event_t event)
{
    return event->timestamp;
}

char* PLUTO_EventPayload(PLUTO_Event_t event)
{
    return event->payload;
}

size_t PLUTO_EventSizeOfPayload(const PLUTO_Event_t event)
{
    return event->nbytes_payload;
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static unsigned int PLUTO_ReadKey(const jsmntok_t *key, const char *data);

bool PLUTO_ReadTopLevelJSON(jsmntok_t *token, size_t size, const char *data, PLUTO_Event_t event)
{
    assert(NULL != token);
    assert(size > 0);
    assert(NULL != data);
    assert(NULL != event);

    event->id = 0U;
    memset(event->payload, '\0', event->nbytes_payload);

    if(JSMN_OBJECT != token[0].type)
    {
        fprintf(stderr, "Toplevel Object of a Message must be a JSON Object.\n");
        return false;
    }
    
    char buffer[1024]; 
    unsigned int result = 0U; 
    for(size_t i=1U;i<size;)
    {
        const unsigned int obj = PLUTO_ReadKey(&token[i], data);
        switch(obj)
        {
            case PLUTO_PARSER_TOKEN_ID:
                memcpy(buffer, data + token[i+1].start, token[i+1].end - token[i+1].start);
                buffer[token[i+1].end - token[i+1].start] = '\0';
                event->id = (int)atoi(buffer);
                printf("  Parse Event Id %u\n", event->id);
                i += 2;
                break;
            case PLUTO_PARSER_TOKEN_EVENT:
                memcpy(buffer, data + token[i+1].start, token[i+1].end - token[i+1].start);
                buffer[token[i+1].end - token[i+1].start] = '\0';
                event->eventid = (int)atoi(buffer);
                printf("  Parse Event Event %u\n", event->eventid);
                i += 2;
                break;
            case PLUTO_PARSER_TOKEN_TIMESTAMP:
                printf("Timestamp %s\n", buffer);
                // TODO: 
                // gettimeofday(&event->timestamp, NULL);
                event->timestamp = 0;
                i += 2;
                break;
            case PARSER_TOKEN_PAYLOAD:
                memcpy(event->payload, data + token[i + 1].start, token[i + 1].end - token[i + 1].start);
                event->payload[token[i + 1].end - token[i + 1].start] = '\0';
                i += 2;
                break;
            default:
                return false;
        }
        result |= obj;
        if(result == 0U)
        {
            return false;
        }
    } 

    return 0x0000000FU == result;
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
        PARSER_TOKEN_PAYLOAD
    };
    
    for(size_t i=0U;i<PLUTO_PARSER_N_KEYS;++i)
    {
        if(strl == expected_strl[i])
        {
            if(0 == memcmp(expected_keys[i], key_buffer, strl))
            {
                printf("Found known Key %s\n", key_buffer);
                return result_values[i];
            }
        }
    }
    printf("Key %s not valid!\n", key_buffer);
    return 0U;
}

