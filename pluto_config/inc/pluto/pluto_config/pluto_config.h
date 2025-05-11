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
    char *url;
    char *name_of_input_queue;
    int32_t number_of_output_queues; 
    char **names_of_output_queues;
    bool manage_inputqueue;
    bool manage_outputqueues;
};
typedef struct PLUTO_Config* PLUTO_Config_t;


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
};
typedef struct PLUTO_CoreConfig* PLUTO_CoreConfig_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_Config_t PLUTO_CreateConfig(const char *filename, const char *name);
void PLUTO_DestroyConfig(PLUTO_Config_t *config);

PLUTO_CoreConfig_t PLUTO_CreateCoreConfig(const char *filename);
void PLUTO_DestroyCoreConfig(PLUTO_CoreConfig_t * config);
size_t PLUTO_CoreConfigNumberOfNodes(const PLUTO_CoreConfig_t config);
PLUTO_Config_t PLUTO_CoreConfigGetNodeConfig(PLUTO_CoreConfig_t config, uint32_t index);

void PLUTO_CoreConfigToString(const PLUTO_CoreConfig_t config, char *buffer, size_t size);
//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
