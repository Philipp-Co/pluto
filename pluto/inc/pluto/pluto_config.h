#ifndef __PLUTO_CONFIG_H__
#define __PLUTO_CONFIG_H__

#include <stdint.h>

struct PLUTO_Config 
{
    char *base_path;
    char *name;
    char *url;
    char *name_of_input_queue;
    int32_t number_of_output_queues; 
    char **names_of_output_queues;
};

typedef struct PLUTO_Config* PLUTO_Config_t;


PLUTO_Config_t PLUTO_CreateConfig(const char *name);
void PLUTO_DestroyConfig(PLUTO_Config_t *config);

#endif
