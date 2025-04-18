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

#include "pluto/pluto_types.h"
#include <pluto/pluto_message_parser.h>
#include <jsmn/jsmn.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define max(x, y)((x) < (y) ? (y) : (x))

#define PLUTO_PARSER_TOKEN_ID 1
#define PLUTO_PARSER_TOKEN_QUEUE 2
#define PLUTO_PARSER_TOKEN_HEADER 4
#define PLUTO_PARSER_TOKEN_PARAMETER 8
#define PLUTO_PARSER_TOKEN_BODY 16


static void PLUTO_PrintRequest(const PLUTO_Request_t request);

bool PLUTO_ReadTopLevelJSON(jsmntok_t *token, size_t size, const char *data, PLUTO_Request_t request);

bool PLUTO_MessageParserLoadRequest(const char *message, PLUTO_Request_t request)
{
    assert(NULL != message);
    assert(NULL != request);
    assert(NULL != request->body && request->max_bytes_body > 0);
    assert(NULL != request->header && request->max_n_header);
    assert(NULL != request->query_parameter && request->max_n_query_parameter);

    snprintf(request->body, request->max_bytes_body, "%s", message);
    request->queue = 0U;
    
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

    snprintf(buffer, size, "{\"id\":%u,\"body\":\"%s\"}", response->id, response->body);
} 

static unsigned int PLUTO_ReadKey(const jsmntok_t *key, const char *data);
static bool PLUTO_ReadStringArray(const jsmntok_t *array, const jsmntok_t *tokens, const char *message, PLUTO_Parameter_t *parameter, size_t *n);

bool PLUTO_ReadTopLevelJSON(jsmntok_t *token, size_t size, const char *data, PLUTO_Request_t request)
{
    assert(NULL != token);
    assert(size > 0);
    assert(NULL != data);
    assert(NULL != request);

    request->n_header = 0;
    request->n_query_parameter = 0;
    request->id = 0U;
    request->queue = 0xFFU;
    memset(request->body, '\0', request->max_bytes_body);

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
            case PLUTO_PARSER_TOKEN_HEADER:
                if(!PLUTO_ReadStringArray(&token[i+1], &token[i + 2], data, request->header, &request->n_header))
                {
                    return false;
                }
                i += max(1, 2 + token[i+1].size);
                break;
            case PLUTO_PARSER_TOKEN_PARAMETER:
                if(!PLUTO_ReadStringArray(&token[i+1], &token[i + 2], data, request->query_parameter, &request->n_query_parameter))
                {
                    return false;
                }
                i += max(1, 2 + token[i+1].size);
                break;
            case PLUTO_PARSER_TOKEN_ID:
                memcpy(buffer, data + token[i+1].start, token[i+1].end - token[i+1].start);
                buffer[token[i+1].end - token[i+1].start] = '\0';
                request->id = (uint8_t)atoi(buffer);
                i += 2;
                break;
            case PLUTO_PARSER_TOKEN_QUEUE:
                memcpy(buffer, data + token[i+1].start, token[i+1].end - token[i+1].start);
                buffer[token[i+1].end - token[i+1].start] = '\0';
                request->queue = (uint8_t)atoi(buffer);
                i += 2;
                break;
            case PLUTO_PARSER_TOKEN_BODY:
                memcpy(request->body, data + token[i + 1].start, token[i + 1].end - token[i + 1].start);
                request->body[token[i + 1].end - token[i + 1].start] = '\0';
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
        
    static const size_t N = 5;
    static const char *expected_keys[N] = 
    {
        "id",
        "queue",
        "header",
        "parameter",
        "body"
    };
    static const unsigned int expected_strl[N] =
    {
        2U,
        5U,
        6U,
        9U,
        4U
    };
    static const unsigned int result_values[N] =
    {
        PLUTO_PARSER_TOKEN_ID,
        PLUTO_PARSER_TOKEN_QUEUE,
        PLUTO_PARSER_TOKEN_HEADER,
        PLUTO_PARSER_TOKEN_PARAMETER,
        PLUTO_PARSER_TOKEN_BODY
    };
    
    for(size_t i=0U;i<N;++i)
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

static bool PLUTO_ReadStringArray(const jsmntok_t *array, const jsmntok_t *tokens, const char *message, PLUTO_Parameter_t *parameter, size_t *n)
{
    char buffer[4096];
    for(int j=0;j<array->size;++j)
    {
        if(JSMN_STRING != tokens[j].type)
        {
            return false;
        }
        const size_t len = tokens[j].end - tokens[j].start;
        memcpy(buffer, message + tokens[j].start, len);
        buffer[len] = '\0'; 
        memcpy(parameter[j].name, buffer, len);
        parameter[j].name[len] = '\0';
    }
    *n = array->size;
    return true;
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
        "  \"queue\": %u,\n",
        request->queue
    );
    printf(
        "  \"header\": [\n"
    );
    for(size_t i=0;i<request->n_header;++i)
    {
        printf(
            "    \"%s\"",
            request->header[i].name
        );
        if(i < request->n_header - 1)
        {
            printf(",\n");
        }
        else
        {
            printf("\n");
        }
    }
    printf("  ],\n");
    printf(
        "  \"query_parameter\": [\n"
    );
    for(size_t i=0;i<request->n_query_parameter;++i)
    {
        printf(
            "    \"%s\"",
            request->query_parameter[i].name
        );
        if(i < request->n_query_parameter - 1)
        {
            printf(",\n");
        }
        else
        {
            printf("\n");
        }
    }
    printf("  ],\n");
    printf(
        "  \"body\": \"%s\"\n",
        request->body
    );
    printf(
        "}\n"
    );
}
