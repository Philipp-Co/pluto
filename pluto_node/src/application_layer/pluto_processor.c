
//
// --------------------------------------------------------------------------------------------------------------------
//

#include "pluto/pluto_event/pluto_event.h"
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

#define PLUTO_PROC_INPUT_QUEUE_PERMISSIONS 0777
#define PLUTO_PROC_OUTPUT_QUEUE_PERMISSIONS 0777

#define PLUTO_PROC_MAX_BYTES_BODY (1024 * 64)
#define PLUTO_PROC_MAX_BYTES_TUPLE_NAME (128)
#define PLUTO_PROC_MAX_BYTES_TUPLE_VALUE (1024 * 16)
#define PLUTO_PROC_MAX_N_HEADER (32)
#define PLUTO_PROC_MAX_N_QUERY_PARAMETER (32)

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_Processor_t PLUTO_CreateProcessor(PLUTO_Config_t config, PLUTO_ProcessCallback_t callback, PLUTO_Logger_t logger)
{
    if(!config) return NULL;
    if(!callback) return NULL;
    PLUTO_Processor_t processor = (PLUTO_Processor_t)malloc(sizeof(struct PLUTO_Processor));
    processor->logger = logger;
    processor->callback = callback;
    processor->info = PLUTO_CreateInfo(config->base_path, config->name);
    
    processor->number_of_output_queues = config->number_of_output_queues;
    processor->output_queues = (PLUTO_MessageQueue_t*)malloc(
        config->number_of_output_queues * sizeof(PLUTO_MessageQueue_t)
    );    
    for(int32_t i=0;i<config->number_of_output_queues;++i)
    {
        PLUTO_LoggerDebug(processor->logger, "Create Message Queue %s", config->names_of_output_queues[i]);
        processor->output_queues[i] = PLUTO_CreateMessageQueue(
            config->base_path,
            config->names_of_output_queues[i],
            PLUTO_PROC_OUTPUT_QUEUE_PERMISSIONS,
            logger
        );
        if(!processor->output_queues[i])
        {
            return NULL;
        }
    }
    processor->input_queue = PLUTO_CreateMessageQueue(
        config->base_path,
        config->name_of_input_queue,
        PLUTO_PROC_INPUT_QUEUE_PERMISSIONS,
        logger
    ); 

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
    struct PLUTO_MsgBuf buffer;
    memset(buffer.text, '\0', sizeof(buffer.text));
    if(
        PLUTO_MessageQueueRead(
            processor->input_queue,
            &buffer
        )
    )
    {
        PLUTO_LoggerInfo(processor->logger, "Processing next Event... %s", buffer.text);
        
        PLUTO_Event_t event = PLUTO_CreateEvent();
        memset(PLUTO_EventPayload(event), '\0', PLUTO_EventSizeOfPayloadBuffer(event));
        
        PLUTO_Event_t output_event = PLUTO_CreateEvent();
        memset(PLUTO_EventPayload(output_event), '\0', PLUTO_EventSizeOfPayloadBuffer(output_event));
        
        bool result = PLUTO_CreateEventFromBuffer(event, buffer.text, sizeof(buffer.text));
        if(!result)
        {
            PLUTO_LoggerWarning(processor->logger, "Error, unable to parse Inputevent!");
            goto end;
        }
        // process...
        PLUTO_ProcessorCallbackInput_t input = {
            .id = event->eventid,
            .event = PLUTO_EventId(event),
            .input_buffer = PLUTO_EventPayload(event),
            .output_buffer = PLUTO_EventPayload(output_event),
            .input_buffer_size = PLUTO_EventSizeOfPayload(event),
            .output_buffer_size = PLUTO_EventSizeOfPayload(output_event),
            .number_of_output_queues = (uint8_t)processor->number_of_output_queues
        };
        PLUTO_ProcessorCallbackOutput_t output = processor->callback(&input);
        //
        // Only send an Event if the Client returned True.
        //
        if(output.return_value)
        {
            if(PLUTO_EventToBuffer(output_event, buffer.text, sizeof(buffer.text) - 1))
            {
                for(int i=0;i<processor->number_of_output_queues;++i)
                {
                    if((1LU << i) & output.output_to_queues)
                    {
                        PLUTO_MessageQueueWrite(
                            processor->output_queues[i],
                            &buffer
                        ); 
                    }
                }
            }
        }

        end:
        PLUTO_DestroyEvent(&output_event);
        if(event) PLUTO_DestroyEvent(&event);
    }
}

//
// --------------------------------------------------------------------------------------------------------------------
//
