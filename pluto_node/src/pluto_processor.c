#include "pluto/pluto_info.h"
#include "pluto/pluto_message_parser.h"
#include "pluto/pluto_message_queue.h"
#include "pluto/pluto_types.h"
#include <pluto/pluto_processor.h>
#include <pluto/pluto_compile_time_switches.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


#define PLUTO_PROC_INPUT_QUEUE_PERMISSIONS 044
#define PLUTO_PROC_OUTPUT_QUEUE_PERMISSIONS 022

#define PLUTO_PROC_MAX_BYTES_BODY (1024 * 64)
#define PLUTO_PROC_MAX_BYTES_TUPLE_NAME (128)
#define PLUTO_PROC_MAX_BYTES_TUPLE_VALUE (1024 * 16)
#define PLUTO_PROC_MAX_N_HEADER (32)
#define PLUTO_PROC_MAX_N_QUERY_PARAMETER (32)

PLUTO_Processor_t PLUTO_CreateProcessor(PLUTO_Config_t config, PLUTO_ProcessCallback_t callback)
{
    PLUTO_Processor_t processor = (PLUTO_Processor_t)malloc(sizeof(struct PLUTO_Processor));

    processor->callback = callback;

    processor->info = PLUTO_CreateInfo(config->base_path, config->name);
    
    processor->number_of_output_queues = config->number_of_output_queues;
    processor->output_queues = (PLUTO_MessageQueue_t*)malloc(
        config->number_of_output_queues * sizeof(PLUTO_MessageQueue_t)
    );    
    for(int32_t i=0;i<config->number_of_output_queues;++i)
    {
        processor->output_queues[i] = PLUTO_CreateMessageQueue(
            config->base_path,
            config->names_of_output_queues[i],
            PLUTO_PROC_OUTPUT_QUEUE_PERMISSIONS
        );
        if(!processor->output_queues[i])
        {
            return NULL;
        }
    }
    processor->input_queue = PLUTO_CreateMessageQueue(
        config->base_path,
        config->name_of_input_queue,
        PLUTO_PROC_INPUT_QUEUE_PERMISSIONS
    ); 

    PLUTO_Request_t request = malloc(sizeof(struct PLUTO_Request));
    request->body = malloc(PLUTO_PROC_MAX_BYTES_BODY);
    request->max_bytes_body = PLUTO_PROC_MAX_BYTES_BODY;
    request->max_size_parameter_name = PLUTO_PROC_MAX_BYTES_TUPLE_NAME;
    request->max_size_parameter_value = PLUTO_PROC_MAX_BYTES_TUPLE_VALUE;
    request->max_n_header = PLUTO_PROC_MAX_N_HEADER;
    request->max_n_query_parameter = PLUTO_PROC_MAX_N_QUERY_PARAMETER;
    request->header = malloc(sizeof(PLUTO_Parameter_t) * PLUTO_PROC_MAX_N_HEADER); 
    request->n_header = PLUTO_PROC_MAX_N_HEADER;
    request->query_parameter = malloc(sizeof(PLUTO_Parameter_t) * PLUTO_PROC_MAX_N_QUERY_PARAMETER);
    request->n_query_parameter = PLUTO_PROC_MAX_N_QUERY_PARAMETER;
    for(size_t i=0;i<PLUTO_PROC_MAX_N_HEADER;++i)
    {
        request->header[i].name = malloc(PLUTO_PROC_MAX_BYTES_TUPLE_NAME);
        request->header[i].value = malloc(PLUTO_PROC_MAX_BYTES_TUPLE_VALUE);
    }
    for(size_t i=0;i<PLUTO_PROC_MAX_N_QUERY_PARAMETER;++i)
    {
        request->query_parameter[i].name = malloc(PLUTO_PROC_MAX_BYTES_TUPLE_NAME);
        request->query_parameter[i].value = malloc(PLUTO_PROC_MAX_BYTES_TUPLE_VALUE);
    }
    processor->request = request;

    PLUTO_InfoValues_t values = {
        .name = config->name,
        .name_of_input_queue = config->name_of_input_queue,
        .n_names_of_output_queues = config->number_of_output_queues,
        .names_of_output_queues = config->names_of_output_queues,
        .n_header = PLUTO_PROC_MAX_N_HEADER,
        .n_query_parameter = PLUTO_PROC_MAX_N_QUERY_PARAMETER
    };
    PLUTO_InfoDisplay(processor->info, &values);

    return processor;
}

void PLUTO_DestroyProcessor(PLUTO_Processor_t *processor)
{
    assert(NULL != processor);
    PLUTO_DestroyMessageQueue(&(*processor)->input_queue);
    for(int32_t i=0;i<(*processor)->number_of_output_queues;++i)
    {
        PLUTO_DestroyMessageQueue(&(*processor)->output_queues[i]);
    }
    PLUTO_DestroyInfo(&(*processor)->info);

    free(*processor);
}

void PLUTO_ProcessorProcess(PLUTO_Processor_t processor)
{
    if(
        PLUTO_MessageQueueRead(
            processor->input_queue,
            processor->request
        )
    )
    {
        if(processor->request->queue >= processor->number_of_output_queues)
        {
            printf("Error! Requested Output Queue does not exist!\n");
        }
        else
        {
            // process...
            PLUTO_Response_t response = alloca(sizeof(struct PLUTO_Response));
            response->body = malloc(processor->request->max_bytes_body);
            memcpy(response->body, processor->request->body, strlen(processor->request->body) + 1);
            response->id = processor->request->id;
            PLUTO_ProcessorCallbackInput_t input = {
                .input_buffer = processor->request->body,
                .output_buffer = response->body,
                .input_buffer_size = processor->request->max_bytes_body,
                .output_buffer_size = processor->request->max_bytes_body
            };
            PLUTO_ProcessorCallbackOutput_t output = processor->callback(&input);
            (void)output;
            PLUTO_MessageQueueWrite(
                processor->output_queues[processor->request->queue],
                response
            ); 
            free(response->body);
        }
    }
}
