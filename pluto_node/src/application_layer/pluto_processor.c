
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/application_layer/pluto_compile_time_switches.h>
#include <pluto/application_layer/pluto_info.h>
#include <pluto/os_abstraction/pluto_types.h>
#include <pluto/application_layer/pluto_processor.h>
#include <pluto/os_abstraction/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_PROC_INPUT_QUEUE_PERMISSIONS 044
#define PLUTO_PROC_OUTPUT_QUEUE_PERMISSIONS 022

#define PLUTO_PROC_MAX_BYTES_BODY (1024 * 64)
#define PLUTO_PROC_MAX_BYTES_TUPLE_NAME (128)
#define PLUTO_PROC_MAX_BYTES_TUPLE_VALUE (1024 * 16)
#define PLUTO_PROC_MAX_N_HEADER (32)
#define PLUTO_PROC_MAX_N_QUERY_PARAMETER (32)

//
// --------------------------------------------------------------------------------------------------------------------
//

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
        printf("Create Output Queue %s\n", config->names_of_output_queues[i]);
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
    request->payload = malloc(PLUTO_PROC_MAX_BYTES_BODY);
    request->max_bytes_payload = PLUTO_PROC_MAX_BYTES_BODY;
    
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
        // process...
        PLUTO_Response_t response = alloca(sizeof(struct PLUTO_Response));
        response->body = malloc(processor->request->max_bytes_payload);
        response->id = 0;
        response->event = 0;
        gettimeofday(&response->timestamp, NULL);
        memcpy(response->body, processor->request->payload, strlen(processor->request->payload) + 1);
        memset(response->body, '\0', processor->request->max_bytes_payload);
        printf("    Request id: %i, event %i\n", processor->request->id, processor->request->event);
        PLUTO_ProcessorCallbackInput_t input = {
            .id = processor->request->id,
            .event = processor->request->event,
            .input_buffer = processor->request->payload,
            .output_buffer = response->body,
            .input_buffer_size = processor->request->max_bytes_payload,
            .output_buffer_size = processor->request->max_bytes_payload
        };
        PLUTO_ProcessorCallbackOutput_t output = processor->callback(&input);
        if(output.return_value)
        {
            memcpy(response->body ,input.output_buffer, output.output_size);
            response->id = output.id;
            response->event = output.event;
            for(int i=0;i<processor->number_of_output_queues;++i)
            {
                printf("Write output to Queue %i\n", i);
                PLUTO_MessageQueueWrite(
                    processor->output_queues[i],
                    response
                ); 
            }
        }
        else
        {
            printf("Callback returned an Error!\n");
        }
        free(response->body);
    }
}

//
// --------------------------------------------------------------------------------------------------------------------
//
