/**
 * \brief   Configuration for Pluto.
 *          Tell Pluto to Create/Destroy Messagequeues or use existing ones.
 *          Set the Name of the Input- and Outputqueues.
 */
#ifndef __PLUTO_CONFIG_H__
#define __PLUTO_CONFIG_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/pluto_logger.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

struct PLUTO_Config 
{
    char *base_path;
    char *name;
    char *name_of_input_queue;
    int32_t number_of_output_queues; 
    char **names_of_output_queues;

    char *python_path; // Optional, because not every Node is a Python Node.
};
typedef struct PLUTO_Config* PLUTO_Config_t;
typedef const struct PLUTO_Config* PLUTO_ConstConfig_t;


#define PLUTO_CORE_CONFIG_NODE_TYPE_PASSTHROUGH 0
#define PLUTO_CORE_CONFIG_NODE_TYPE_PYTHON 1
#define PLUTO_CORE_CONFIG_NODE_TYPE_SHARED_LIB 2

struct PLUTO_CoreConfigNode
{
    char *name;
    char *filename;
    char *executable;
    uint32_t type;
};

struct PLUTO_CoreConfig
{
    struct PLUTO_CoreConfigNode *nodes;
    PLUTO_Config_t *configurations;
    size_t n_configurations;
    PLUTO_Logger_t logger;
};
typedef struct PLUTO_CoreConfig* PLUTO_CoreConfig_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_Config_t PLUTO_CreateConfig(const char *filename, const char *name, PLUTO_Logger_t logger);
PLUTO_Config_t PLUTO_CopyConfig(const PLUTO_Config_t config);
void PLUTO_DestroyConfig(PLUTO_Config_t *config);
bool PLUTO_ConfigIsPythonPathSet(const PLUTO_Config_t config);
size_t PLUTO_ConfigMaxNumberOfCharactersForPythonPath(void);
size_t PLUTO_ConfigMaxNumberOfOutputQueues(void);
int32_t PLUTO_ConfigNumberOfOutputQueues(const PLUTO_Config_t config);
const char* PLUTO_ConfigNameOfOutputQueue(const PLUTO_Config_t config, const int32_t index);
const char* PLUTO_ConfigNameOfInputQueue(const PLUTO_Config_t config);
const char* PLUTO_ConfigPythonPath(const PLUTO_Config_t config);

PLUTO_CoreConfig_t PLUTO_CreateCoreConfig(const char *filename, PLUTO_Logger_t logger);
void PLUTO_DestroyCoreConfig(PLUTO_CoreConfig_t * config);
size_t PLUTO_CoreConfigNumberOfNodes(const PLUTO_CoreConfig_t config);
PLUTO_Config_t PLUTO_CoreConfigGetNodeConfig(PLUTO_CoreConfig_t config, uint32_t index);

void PLUTO_CoreConfigToString(const PLUTO_CoreConfig_t config, char *buffer, size_t size);
//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
