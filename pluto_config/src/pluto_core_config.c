
//
// --------------------------------------------------------------------------------------------------------------------
//

#include "pluto/os_abstraction/pluto_logger.h"
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_malloc.h>
#include <pluto/pluto_config/private/pluto_file.h>

#include <jsmn/jsmn.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

static PLUTO_CoreConfig_t PLUTO_CONFIG_ParseFile(const PLUTO_CONFIG_Buffer_t buffer, PLUTO_Logger_t logger);

//
// --------------------------------------------------------------------------------------------------------------------
//
void PLUTO_CoreConfigToString(const PLUTO_CoreConfig_t config, char *buffer, size_t size)
{
    memset(buffer, '\0', size);
    const int result = snprintf(
        buffer,
        size,
        "Core-Config:\n"
        "  Number of Configurations: %lu\n"
        "  Nodes:\"",
        config->n_configurations
    );
    int sum = result; 
    for(size_t i=0;i<config->n_configurations;++i)
    {
        const int len0 = strlen(config->nodes[i].name);
        const int len1 = strlen(config->nodes[i].filename);
        const int len2 = strlen(config->nodes[i].executable);
        const int typelen = strlen("python");
        static const char *name_field = "\n    name: ";
        static const char *cfile_field = "\n    configuration-file: ";
        static const char *executable_field = "\n    executable: ";
        static const char *type_field = "\n    type: ";
        const int clen = strlen(name_field) + strlen(cfile_field) + strlen(executable_field) + strlen(type_field) + 1;
        if((sum + len0 + len1 + len2 + clen) < (int)(size-1))
        {
            strcat(buffer, name_field);
            strcat(buffer, config->nodes[i].name);
            strcat(buffer, cfile_field);
            strcat(buffer, config->nodes[i].filename);
            strcat(buffer, executable_field);
            strcat(buffer, config->nodes[i].executable);
            strcat(buffer, type_field);
            if(config->nodes[i].type == PLUTO_CORE_CONFIG_NODE_TYPE_PYTHON)
            {
                strcat(buffer, "python");
            }
            else if (PLUTO_CORE_CONFIG_NODE_TYPE_SHARED_LIB == config->nodes[i].type)
            {
                strcat(buffer, "shared");
            }
            else
            {
                strcat(buffer, "-");
            }
            strcat(buffer, "\n");
            sum += (len0 + len1 + len2 + typelen + clen);
        }
        else
        {
            memset(buffer, '\0', size);
            strcat(buffer, "Error!\n");
            return;
        }
    }
}

PLUTO_CoreConfig_t PLUTO_CreateCoreConfig(const char *filename, PLUTO_Logger_t logger)
{
    // "buffer" is malloced by Function
    PLUTO_CONFIG_Buffer_t buffer = PLUTO_CONFIG_ReadFile(filename, logger);
    if(!buffer)
    {
        return NULL;
    }
    PLUTO_CoreConfig_t config = PLUTO_CONFIG_ParseFile(buffer, logger);
    // free "buffer"!
    PLUTO_CONFIG_DestroyBuffer(&buffer);
    return config;
}

void PLUTO_DestroyCoreConfig(PLUTO_CoreConfig_t * config)
{
    for(size_t i=0;i<(*config)->n_configurations;++i)
    {
        if((*config)->configurations && (*config)->configurations[i]) PLUTO_DestroyConfig(&(*config)->configurations[i]);
        if((*config)->nodes)
        {
            PLUTO_Free((*config)->nodes[i].name);
            PLUTO_Free((*config)->nodes[i].filename);
            PLUTO_Free((*config)->nodes[i].executable);
        }
    }
    if((*config)->configurations) PLUTO_Free((*config)->configurations);
    if((*config)->nodes) PLUTO_Free((*config)->nodes);
    PLUTO_Free(*config);
    *config = NULL;
}

PLUTO_Config_t PLUTO_CoreConfigGetNodeConfig(PLUTO_CoreConfig_t config, uint32_t index)
{
    if(index >= (uint32_t)PLUTO_CoreConfigNumberOfNodes(config))
    {
        return NULL;
    }

    if(!config->configurations[index])
    {
        //
        // load config from file.
        //
        PLUTO_LoggerInfo(
            config->logger, "Node Filename: %s, Config Node Name: %s", config->nodes[index].filename, config->nodes[index].name
        );
        config->configurations[index] = PLUTO_CreateConfig(
            config->nodes[index].filename, config->nodes[index].name, config->logger
        ); 
    }
    return config->configurations[index];
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static bool PLUTO_CONFIG_ParseNode(
    PLUTO_CoreConfig_t,
    PLUTO_CONFIG_Buffer_t buffer,
    jsmntok_t *object_token
);

static bool PLUTO_CONFIG_ParseNodes(
    PLUTO_CoreConfig_t,
    PLUTO_CONFIG_Buffer_t buffer,
    jsmntok_t *token,
    size_t ntoken
);

static PLUTO_CoreConfig_t PLUTO_CONFIG_ParseFile(const PLUTO_CONFIG_Buffer_t buffer, PLUTO_Logger_t logger)
{
    assert(NULL != buffer);
    assert(NULL != buffer->buffer);
    //
    // {
    //   "nodes": [
    //      {
    //          "type": str,
    //          "name": str,
    //          "configuration-file": str,
    //          "executable": str,
    //      },
    //      ...
    //   ]
    // }
    //
    //
    PLUTO_CoreConfig_t core_config = PLUTO_Malloc(
        sizeof(struct PLUTO_CoreConfig)
    );
    core_config->configurations = NULL;
    core_config->nodes = NULL;
    core_config->n_configurations = 0;
    core_config->logger = logger;

    char key[1024];
    jsmntok_t token[128];
    jsmn_parser parser;
    jsmn_init(&parser);
    
    const int result = jsmn_parse(&parser, buffer->buffer, buffer->nbytes, token, sizeof(token));
    if(result < 3)
    {
        char *tmp = PLUTO_Malloc(buffer->nbytes + 1);
        memset(tmp, '\0', buffer->nbytes + 1);
        memcpy(tmp, buffer->buffer, buffer->nbytes);
        PLUTO_LoggerError(logger, "JSMN Error: %i; Buffer was: %s", result, tmp);
        PLUTO_Free(tmp);
        goto error;
    }

    if(JSMN_OBJECT != token[0].type)
    {
        PLUTO_LoggerError(logger, "Error during Config parsing: Top-Level Object is no JSON Object...");
        goto error;
    }

    const int length = token[1].end - token[1].start;
    memcpy(key, ((char*)buffer->buffer) + token[1].start, length);
    key[length] = '\0';
    if(0 != strcmp("nodes", key))
    {
        goto error;
    }

    if(!PLUTO_CONFIG_ParseNodes(core_config, buffer, &token[2], (result - 2)))
    {
        PLUTO_DestroyCoreConfig(&core_config);
        return NULL;
    }
    
    char pbuffer[4096];
    PLUTO_CoreConfigToString(core_config, pbuffer, sizeof(pbuffer));
    return core_config;

error:
    PLUTO_Free(core_config);
    return NULL;
}

size_t PLUTO_CoreConfigNumberOfNodes(const PLUTO_CoreConfig_t config)
{
    return config->n_configurations;
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static bool PLUTO_CONFIG_ParseNode(
    PLUTO_CoreConfig_t config,
    PLUTO_CONFIG_Buffer_t buffer,
    jsmntok_t *token
)
{
    unsigned int required_attributes = 0xFU;

    if(JSMN_OBJECT != token[0].type)
    {
        PLUTO_LoggerError(config->logger, "Error: Unable to parse Config Object from non Object Type...");
        return false;
    }
    
    size_t index = config->n_configurations;
    //
    // jsmn size is the number of Items in a Object:
    // {"a": x} -> Size = 1
    // {"a": x, "b": y, "c": z} -> Size = 3
    //
    for(int i=0;i<(token[0].size*2); i+=2)
    {
        jsmntok_t *key_token = &token[i+1];
        jsmntok_t *value_token = &token[i+2];

        if(JSMN_STRING != key_token->type || JSMN_STRING != value_token->type)
        {
            PLUTO_LoggerError(config->logger, "Error: Unable to parse Key/Value Pair.");
            return false;
        }

        const char *start = ((char*)buffer->buffer) + key_token->start;

        const char *value_start = ((char*)buffer->buffer) + value_token->start;
        const int value_length = value_token->end - value_token->start;
        
        if(0 == memcmp("name", start, strlen("name")))
        {
            config->nodes[index].name = PLUTO_Malloc(value_length + 1);
            memcpy(config->nodes[index].name, value_start, value_length);
            config->nodes[index].name[value_length] = '\0';

            required_attributes ^= 0x1U;
        }
        else if(0 == memcmp("type", start, strlen("type")))
        {
            const int ptlen = strlen("passthrough");
            const int pylen = strlen("python");
            const int shlen = strlen("shared");
            if(value_length > ptlen)
            {
                PLUTO_LoggerError(config->logger, "Unknown Object Node Type!");
                return false;
            }
            if(0 == memcmp("python", value_start, pylen))
            {
                config->nodes[index].type = PLUTO_CORE_CONFIG_NODE_TYPE_PYTHON;
            }
            else if(0 == memcmp("shared", value_start, shlen))
            {
                config->nodes[index].type = PLUTO_CORE_CONFIG_NODE_TYPE_SHARED_LIB;
            }
            else if(0 == memcmp("passthrough", value_start, ptlen))
            {
                config->nodes[index].type = PLUTO_CORE_CONFIG_NODE_TYPE_PASSTHROUGH;
            }
            else
            {
                config->nodes[index].type = PLUTO_CORE_CONFIG_NODE_TYPE_PASSTHROUGH;
            }

            required_attributes ^= 0x2U;
        }
        else if(0 == memcmp("configuration-file", start, strlen("configuration-file")))
        {
            config->nodes[index].filename = PLUTO_Malloc(value_length + 1);
            memcpy(config->nodes[index].filename, value_start, value_length);
            config->nodes[index].filename[value_length] = '\0';
            
            required_attributes ^= 0x4U;
        }
        else if(0 == memcmp("executable", start, strlen("executable")))
        {
            config->nodes[index].executable = PLUTO_Malloc(value_length + 1);
            memcpy(config->nodes[index].executable, value_start, value_length);
            config->nodes[index].executable[value_length] = '\0';
            
            required_attributes ^= 0x8U;
        }
        else 
        {
            PLUTO_LoggerError(config->logger, "Error: Unknown Key!");
            return false;
        }
    }
    config->n_configurations++;
    return 0x0U == required_attributes;
}

static bool PLUTO_CONFIG_ParseNodes(
    PLUTO_CoreConfig_t config,
    PLUTO_CONFIG_Buffer_t buffer,
    jsmntok_t *token,
    size_t ntoken
)
{
    if(ntoken <= 0)
    {
        return false;
    }

    if(JSMN_ARRAY != token[0].type)
    {
        PLUTO_LoggerError(config->logger, "Expected Array while parsing Nodes...");
        return false;
    }

    const size_t n = (size_t)token[0].size;
    config->configurations = PLUTO_Malloc(n * sizeof(PLUTO_Config_t));
    config->nodes = PLUTO_Malloc(n * sizeof(struct PLUTO_CoreConfigNode));
    config->n_configurations = 0;
    
    //
    // Init all Configuration Orbjects to NULL...
    // 
    for(size_t j=0;j<n;++j)
    {
        config->configurations[j] = NULL;
    }
    
    //
    // Read Input.
    //
    int token_index = 1;
    for(int i=0;i<token[0].size;++i)
    {
        jsmntok_t *object = &token[token_index];
        if(JSMN_OBJECT != object->type || (4 != object->size))
        {
            PLUTO_LoggerError(
                config->logger, 
                "Error unable to parse Nodes: Node Type is %i, Object Size is %i\n",
                object->type, object->size
            );
            return false;
        }
        if(!PLUTO_CONFIG_ParseNode(config, buffer, object))
        {
            PLUTO_LoggerError(
                config->logger,
                "Error Unable to parse Node Config."
            );
            return false;
        }
        //
        // +1 for the Object token
        // + 6 for the Object Attributes
        // +1 to point to the Next Object Token.
        //
        ++token_index;
        while(JSMN_OBJECT != token[token_index].type)
        {
            ++token_index;
            if((size_t)token_index >= ntoken)
            {
                PLUTO_LoggerInfo(
                    config->logger,
                    "No more Nodes Configs found. Exiting Loop..."
                );
                return true;
            }
        }
    }
    return true;
}
