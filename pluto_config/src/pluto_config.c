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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES 4
#define PLUTO_CONFIG_MAX_STRLEN_PYTHON_PATH ((4096 * 4) - 1)
#define PLUTO_CONFIG_MAX_STRLEN_NAME (1024 - 1)
#define PLUTO_CONFIG_MAX_STRLEN_BASE_PATH (4096 - 1)
#define PLUTO_CONFIG_MAX_STRLEN_NAME_OF_QUEUE (1024 - 1)
//
// --------------------------------------------------------------------------------------------------------------------
//

static PLUTO_Config_t PLUTO_ConfigMallocConfigObject(void);
static bool PLUTO_ReadConfigFromFile(PLUTO_Config_t config, const char *filename, PLUTO_Logger_t logger);

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_Config_t PLUTO_CreateConfig(const char *filename, const char *name, PLUTO_Logger_t logger)
{
    assert(strlen(name) < PLUTO_CONFIG_MAX_STRLEN_NAME);

    PLUTO_Config_t config = PLUTO_ConfigMallocConfigObject();
    
    memset(config->name, '\0', strlen(name) + 1);
    memcpy(config->name, name, strlen(name));
   
    if(!PLUTO_ReadConfigFromFile(config, filename, logger))
    {
        PLUTO_DestroyConfig(&config);
    }
    return config;
}

void PLUTO_DestroyConfig(PLUTO_Config_t *config)
{
    if(!(*config))
    {
        return;
    }

    PLUTO_Free(
        (*config)->base_path
    );
    PLUTO_Free(
        (*config)->name
    );
    PLUTO_Free(
        (*config)->name_of_input_queue
    );
    PLUTO_Free(
        (*config)->python_path
    );
    for(int32_t i=0;i<PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES;++i)
    {
        PLUTO_Free((*config)->names_of_output_queues[i]);
    }
    PLUTO_Free(
        (*config)->names_of_output_queues
    );
    PLUTO_Free(*config);
    *config = NULL;
}

bool PLUTO_ConfigIsPythonPathSet(const PLUTO_Config_t config)
{
    assert(NULL != config);
    assert(NULL != config->python_path);
    return 0LU < strlen(
        config->python_path
    );
}

size_t PLUTO_ConfigMaxNumberOfCharactersForPythonPath(void)
{
    return (size_t)PLUTO_CONFIG_MAX_STRLEN_PYTHON_PATH;
}

size_t PLUTO_ConfigMaxNumberOfOutputQueues(void)
{
    return (size_t)PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES;
}

int32_t PLUTO_ConfigNumberOfOutputQueues(const PLUTO_Config_t config)
{
    return config->number_of_output_queues;
}

const char* PLUTO_ConfigPythonPath(const PLUTO_Config_t config)
{
    return config->python_path;
}

const char* PLUTO_ConfigNameOfOutputQueue(const PLUTO_Config_t config, const int32_t index)
{
    assert(index >= 0);
    assert(index < config->number_of_output_queues);
    assert(NULL != config->names_of_output_queues[index]);
    return config->names_of_output_queues[index];
}

const char* PLUTO_ConfigNameOfInputQueue(const PLUTO_Config_t config)
{
    assert(NULL != config);
    assert(NULL != config->name_of_input_queue);
    return config->name_of_input_queue;
}
//
// --------------------------------------------------------------------------------------------------------------------
//


static char* PLUTO_ReadyConfigFromFile1(const char *filename, PLUTO_Logger_t logger);
static bool PLUTO_ParseConfig(PLUTO_Config_t config, const char *bytes, PLUTO_Logger_t logger);

static bool PLUTO_ReadConfigFromFile(PLUTO_Config_t config, const char *filename, PLUTO_Logger_t logger)
{
    PLUTO_LoggerInfo(
        logger,
        "Read Config from File %s", filename
    );
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
    long size = ftell(file);
    if(size <= 0)
    {
        PLUTO_LoggerError(logger, "File has size 0.");
        fclose(file);
        return NULL;
    }
    PLUTO_LoggerInfo(logger, "Config File size is %lu.", size);
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
                        char *dest = config->names_of_output_queues[j]; 
                        const char *src = bytes + token[i + j + 2].start;
                        memcpy(
                            dest,
                            src,
                            token[i + j + 2].end - token[i + j + 2].start
                        );
                        config->names_of_output_queues[j][token[i + j + 2].end - token[i + j + 2].start] = '\0'; 
                        PLUTO_LoggerInfo(logger, "    Outputqueue Name: %s", config->names_of_output_queues[j]);
                    }
                    flags |= 0x4U;
                    i += token[i + 1].size + 2;
                }
                else if(0 == strcmp("python_path", key))
                {
                    PLUTO_LoggerInfo(logger, "  python_path: %s", value);
                    const size_t strl = strlen(value);
                    memcpy(
                        config->python_path, value, strl > PLUTO_ConfigMaxNumberOfCharactersForPythonPath() ? PLUTO_ConfigMaxNumberOfCharactersForPythonPath() : strl
                    );
                    i += 2;
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
static PLUTO_Config_t PLUTO_ConfigMallocConfigObject(void)
{
    PLUTO_Config_t config = (PLUTO_Config_t)PLUTO_Malloc(sizeof(struct PLUTO_Config));
    
    config->name = PLUTO_Malloc(PLUTO_CONFIG_MAX_STRLEN_NAME + 1);
    memset(config->name, '\0', PLUTO_CONFIG_MAX_STRLEN_NAME + 1);
   
    config->python_path = PLUTO_Malloc(PLUTO_CONFIG_MAX_STRLEN_PYTHON_PATH + 1); 
    memset(config->python_path, '\0', PLUTO_CONFIG_MAX_STRLEN_PYTHON_PATH + 1);

    config->base_path = PLUTO_Malloc(PLUTO_CONFIG_MAX_STRLEN_BASE_PATH + 1);
    memset(config->base_path, '\0', PLUTO_CONFIG_MAX_STRLEN_BASE_PATH + 1);

    config->name_of_input_queue = PLUTO_Malloc(PLUTO_CONFIG_MAX_STRLEN_NAME_OF_QUEUE + 1);
    memset(config->name_of_input_queue, '\0', PLUTO_CONFIG_MAX_STRLEN_NAME_OF_QUEUE + 1);
    
    config->names_of_output_queues = (char**)PLUTO_Malloc(
        sizeof(char*) * PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES
    ); 
    for(size_t i=0;i<PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES;++i)
    {
        config->names_of_output_queues[i] = (char*)PLUTO_Malloc(
            PLUTO_CONFIG_MAX_STRLEN_NAME_OF_QUEUE + 1
        );
    }
    return config;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
