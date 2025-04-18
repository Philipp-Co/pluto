#ifndef __PLUTO_PROCESSOR_H__
#define __PLUTO_PROCESSOR_H__

#include <pluto/pluto_config.h>
#include <pluto/pluto_message_queue.h>
#include <pluto/pluto_info.h>

struct PLUTO_Processor
{
    PLUTO_Info_t info;
    PLUTO_MessageQueue_t input_queue;
    int32_t number_of_output_queues;
    PLUTO_MessageQueue_t *output_queues;
    PLUTO_Request_t request;
}; 
typedef struct PLUTO_Processor* PLUTO_Processor_t;

PLUTO_Processor_t PLUTO_CreateProcessor(PLUTO_Config_t config);
void PLUTO_DestroyProcessor(PLUTO_Processor_t *processor);
void PLUTO_ProcessorProcess(PLUTO_Processor_t processor);

#endif
