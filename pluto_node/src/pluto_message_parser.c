/**
 *
 *  Schema?
 *      {
 *          "id": unsigned int,
 *          "queue": unsigned int,
 *          "header": [
 *              str
 *          ],
 *          "parameter": [
 *              str
 *          ],
 *          "body": str
 *      }
 */

//
// --------------------------------------------------------------------------------------------------------------------
//

#include "pluto/pluto_types.h"
#include <pluto/pluto_message_parser.h>
#include <time.h>

#define JSMN_HEADER
#include <jsmn/jsmn.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

#define max(x, y)((x) < (y) ? (y) : (x))

#define PLUTO_PARSER_TOKEN_ID 1
#define PLUTO_PARSER_TOKEN_EVENT 2
#define PLUTO_PARSER_TOKEN_TIMESTAMP 4
#define PARSER_TOKEN_PAYLOAD 8

//
// --------------------------------------------------------------------------------------------------------------------
//

static void PLUTO_PrintRequest(const PLUTO_Request_t request);
bool PLUTO_ReadTopLevelJSON(jsmntok_t *token, size_t size, const char *data, PLUTO_Request_t request);

//
// --------------------------------------------------------------------------------------------------------------------
//

bool PLUTO_MessageParserLoadRequest(const char *message, PLUTO_Request_t request)
{
    assert(NULL != message);
    assert(NULL != request);

    snprintf(request->payload, request->max_bytes_payload, "%s", message);
    request->event = 0U;
    request->id = 0U;
    
    jsmntok_t token[128];
    jsmn_parser parser;
    jsmn_init(&parser);
    
    const int result = jsmn_parse(&parser, message, strlen(message), token, sizeof(token));
    if(result > 0)
    {
        if(!PLUTO_ReadTopLevelJSON(token, result, message, request))
        {
            return false;
        }
        return true;
    }
    return false;
}

void PLUTO_MessageParserDumpResponse(const PLUTO_Response_t response, char *buffer, int32_t size)
{
    assert(NULL != response);
    assert(NULL != buffer);
    assert(size > 0);
    
    char time_buffer[4096];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%dT%H:%M:%S", gmtime(&response->timestamp.tv_sec));
    snprintf(buffer, size, "{\"id\":%i,\"event\":%i,\"time\":\"%s.%i+0000\",\"payload\":\"%s\"}", response->id, response->event, time_buffer, response->timestamp.tv_usec, response->body);
} 

//
// --------------------------------------------------------------------------------------------------------------------
//

static unsigned int PLUTO_ReadKey(const jsmntok_t *key, const char *data);

bool PLUTO_ReadTopLevelJSON(jsmntok_t *token, size_t size, const char *data, PLUTO_Request_t request)
{
    assert(NULL != token);
    assert(size > 0);
    assert(NULL != data);
    assert(NULL != request);

    request->id = 0U;
    memset(request->payload, '\0', request->max_bytes_payload);

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
                request->id = (int)atoi(buffer);
                printf("  Parse Event Id %u\n", request->id);
                i += 2;
                break;
            case PLUTO_PARSER_TOKEN_EVENT:
                memcpy(buffer, data + token[i+1].start, token[i+1].end - token[i+1].start);
                buffer[token[i+1].end - token[i+1].start] = '\0';
                request->event = (int)atoi(buffer);
                printf("  Parse Event Event %u\n", request->id);
                i += 2;
                break;
            case PLUTO_PARSER_TOKEN_TIMESTAMP:
                printf("Timestamp %s\n", buffer);
                gettimeofday(&request->timestamp, NULL);
                i += 2;
                break;
            case PARSER_TOKEN_PAYLOAD:
                memcpy(request->payload, data + token[i + 1].start, token[i + 1].end - token[i + 1].start);
                request->payload[token[i + 1].end - token[i + 1].start] = '\0';
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

    PLUTO_PrintRequest(request);
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

static void PLUTO_PrintRequest(const PLUTO_Request_t request)
{
    printf(
        "{\n"
    );
    printf(
        "  \"id\": %u,\n",
        request->id
    );
    printf(
        "  \"event\": %u,\n",
        request->event
    );
    printf(
        "  \"time\": %lu,\n",
        request->timestamp.tv_sec
    );
    printf(
        "  \"payload\": \"%s\"\n",
        request->payload
    );
    printf(
        "}\n"
    );
}
//
// --------------------------------------------------------------------------------------------------------------------
//
