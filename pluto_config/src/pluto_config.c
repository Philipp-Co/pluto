/**
 *
 * \brief   Schema of Configuration:
 *              {
 *                  "manage_inputqueue": bool,
 *                  "manage_outputqueues": bool,
 *                  "base_path": str,
 *                  "name": str,
 *                  "name_of_inputqueue": str,
 *                  "name_of_outputqueues": [
 *                      str
 *                  ],
 *                  "dll_path": str
 *              }
 *
 */

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#define JSMN_HEADER
#include <jsmn/jsmn.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES 4

//
// --------------------------------------------------------------------------------------------------------------------
//

static bool PLUTO_ReadConfigFromFile(PLUTO_Config_t config, const char *filename, PLUTO_Logger_t logger);

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_Config_t PLUTO_CreateConfig(const char *filename, const char *name, PLUTO_Logger_t logger)
{
    PLUTO_Config_t config = (PLUTO_Config_t)PLUTO_Malloc(sizeof(struct PLUTO_Config));
    
    config->manage_inputqueue = true;
    config->manage_outputqueues = true;

    config->name = PLUTO_Malloc(strlen(name) + 1);
    memset(config->name, '\0', strlen(name) + 1);
    memcpy(config->name, name, strlen(name));
   
    config->base_path = PLUTO_Malloc(4096);
    memset(config->base_path, '\0', 4096);

    char *buffer = PLUTO_Malloc(4096);
    config->name_of_input_queue = buffer;
    
    config->names_of_output_queues = (char**)PLUTO_Malloc(
        sizeof(char*) * PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES
    ); 
    for(size_t i=0;i<PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES;++i)
    {
        config->names_of_output_queues[i] = PLUTO_Malloc(
            4096
        );
        snprintf(
            config->names_of_output_queues[i], 
            4096, 
            "%s_oq_%lu", 
            config->name, 
            i
        );
    }
   
    if(!PLUTO_ReadConfigFromFile(config, filename, logger))
    {
        PLUTO_Free(config);
        return NULL;
    }
    config->url = NULL;
    return config;
}

void PLUTO_DestroyConfig(PLUTO_Config_t *config)
{
    //if((*config)->name_of_input_queue) PLUTO_Free((*config)->name_of_input_queue);
    if((*config)->url) PLUTO_Free((*config)->url);
    for(int32_t i=0;i<(*config)->number_of_output_queues;++i)
    {
        if((*config)->names_of_output_queues[i]) PLUTO_Free((*config)->names_of_output_queues[i]);
    }
    PLUTO_Free(*config);
    *config = NULL;
}

//
// --------------------------------------------------------------------------------------------------------------------
//


static char* PLUTO_ReadyConfigFromFile1(const char *filename, PLUTO_Logger_t logger);
static bool PLUTO_ParseConfig(PLUTO_Config_t config, const char *bytes, PLUTO_Logger_t logger);

static bool PLUTO_ReadConfigFromFile(PLUTO_Config_t config, const char *filename, PLUTO_Logger_t logger)
{
    bool return_value = false;
    char *files_content = PLUTO_ReadyConfigFromFile1(filename, logger);
    if(!files_content)
    {
        return false;
    }
    return_value = PLUTO_ParseConfig(config, files_content, logger);
    PLUTO_Free(files_content);
    return return_value;
}

static char* PLUTO_ReadyConfigFromFile1(const char *filename, PLUTO_Logger_t logger)
{
    PLUTO_LoggerInfo(logger, "Read Config File: %s", filename);
    FILE *file = fopen(filename, "r");
    if(!file)
    {
        PLUTO_LoggerError(logger, "Unable to open Config File.");
        return NULL;
    }
    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file) + 1;
    if(size == 0LU)
    {
        PLUTO_LoggerError(logger, "File has size 0.");
        fclose(file);
        return NULL;
    }
    fseek(file, 0L, SEEK_SET);
    char *buffer = PLUTO_Malloc(size + 1);
    memset(buffer, '\0', size + 1);

    size_t result = fread(buffer, 1, size, file);
    if(result == 0)
    {
        PLUTO_LoggerError(logger, "Result was 0.");
        PLUTO_Free(buffer);
    }
    fclose(file);
    return buffer;
}

static bool PLUTO_ParseConfig(PLUTO_Config_t config, const char *bytes, PLUTO_Logger_t logger)
{
    unsigned int flags = 0U;
    jsmntok_t token[128];
    jsmn_parser parser;
    jsmn_init(&parser);
    
    const int result = jsmn_parse(
        &parser, 
        bytes, 
        strlen(bytes), 
        token, 
        sizeof(token)
    );

    PLUTO_LoggerInfo(logger, "Parse Config...");
    if(result > 0)
    {
        char *key = PLUTO_Malloc(1024);
        char *value = PLUTO_Malloc(4096);
        for(int i=1;i<result;)
        {
            memcpy(key, bytes + token[i].start, token[i].end - token[i].start);
            key[token[i].end - token[i].start] = '\0';
            if(JSMN_STRING == token[i].type)
            {
                memcpy(value, bytes + token[i + 1].start, token[i + 1].end - token[i + 1].start);
                value[token[i + 1].end - token[i + 1].start] = '\0';
                if(0 == strcmp("work_dir", key))
                {
                    PLUTO_LoggerInfo(logger, "  work_dir: %s", value);
                    memcpy(config->base_path, value, strlen(value) + 1);
                    flags |= 0x1U;
                    i += 2;
                }
                else if(0 == strcmp("name_of_input_queue", key))
                {
                    PLUTO_LoggerInfo(logger, "  input_queue_name: %s", value);
                    memcpy(config->name_of_input_queue, value, strlen(value) + 1);
                    flags |= 0x2U;
                    i += 2;
                }
                else if(0 == strcmp("names_of_output_queues", key))
                {
                    PLUTO_LoggerInfo(logger, "  output_queue_names: %s", value);
                    PLUTO_LoggerInfo(logger, "  number_of_output_queues: %i", token[i + 1].size);
                    config->number_of_output_queues = token[i + 1].size;
                    for(int j=0;j<token[i + 1].size;++j)
                    {
                        memcpy(
                            config->names_of_output_queues[j], 
                            bytes + token[i + j + 2].start,
                            token[i + j + 2].end - token[i + j + 2].start
                        );
                        config->names_of_output_queues[j][token[i + j + 2].end - token[i + j + 2].start] = '\0'; 
                        PLUTO_LoggerInfo(logger, "    Outputqueue Name: %s", config->names_of_output_queues[j]);
                    }
                    flags |= 0x4U;
                    i += token[i + 1].size + 2;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        PLUTO_Free(key);
        PLUTO_Free(value);
    }
    return 0x7U == flags;
}

//
// --------------------------------------------------------------------------------------------------------------------
//
