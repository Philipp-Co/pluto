
#include <pluto/pluto_config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES 4

PLUTO_Config_t PLUTO_CreateConfig(const char *name)
{
    PLUTO_Config_t config = (PLUTO_Config_t)malloc(sizeof(struct PLUTO_Config));

    config->base_path = "/Users/philippkroll/Repositories/pluto/build/.pluto/ipc/";

    config->name = malloc(strlen(name) + 1);
    memset(config->name, '\0', strlen(name) + 1);
    memcpy(config->name, name, strlen(name));
    
    char *buffer = malloc(4096);
    snprintf(buffer, 4096, "%s_iq", config->name);
    config->name_of_input_queue = buffer;
    config->number_of_output_queues = PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES;
    config->names_of_output_queues = (char**)malloc(
        sizeof(char*) * PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES
    ); 
    for(size_t i=0;i<PLUTO_CONFIG_NUMBER_OF_OUTPUT_QUEUES;++i)
    {
        config->names_of_output_queues[i] = malloc(
            256
        );
        snprintf(
            config->names_of_output_queues[i], 
            256, 
            "%s_oq_%lu", 
            config->name, 
            i
        );
    }
    config->url = NULL;

    return config;
}

void PLUTO_DestroyConfig(PLUTO_Config_t *config)
{
    //if((*config)->name_of_input_queue) free((*config)->name_of_input_queue);
    if((*config)->url) free((*config)->url);
    for(int32_t i=0;i<(*config)->number_of_output_queues;++i)
    {
        if((*config)->names_of_output_queues[i]) free((*config)->names_of_output_queues[i]);
    }
    free(*config);
    *config = NULL;
}
