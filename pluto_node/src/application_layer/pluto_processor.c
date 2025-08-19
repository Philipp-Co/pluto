
//
// --------------------------------------------------------------------------------------------------------------------
//

#include "pluto/os_abstraction/files/pluto_file.h"
#include "pluto/os_abstraction/pluto_time.h"
#include "pluto/os_abstraction/signals/pluto_signal.h"
#include "pluto/os_abstraction/system_events/pluto_system_events.h"
#include "pluto/pluto_event/pluto_event.h"
#include <pluto/application_layer/pluto_compile_time_switches.h>
#include <pluto/application_layer/pluto_info.h>
#include <pluto/os_abstraction/pluto_types.h>
#include <pluto/application_layer/pluto_processor.h>
#include <pluto/os_abstraction/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_malloc.h>
#include <pluto/application_layer/events/event_definitions.h>

#include <unistd.h>
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

PLUTO_Processor_t PLUTO_CreateProcessor(
    PLUTO_Config_t config, 
    PLUTO_SignalHandler_t signal_handler,
    PLUTO_ProcessCallback_t callback, 
    PLUTO_Logger_t logger
)
{
    if(!config) return NULL;
    if(!callback) return NULL;
    PLUTO_Processor_t processor = (PLUTO_Processor_t)PLUTO_Malloc(sizeof(struct PLUTO_Processor));
    processor->logger = logger;
    processor->signal_handler = signal_handler;
    processor->callback = callback;
    
    processor->number_of_output_queues = config->number_of_output_queues;
    processor->output_queues = (PLUTO_MessageQueue_t*)PLUTO_Malloc(
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
            PLUTO_Free(processor->output_queues);
            PLUTO_Free(processor);
            return NULL;
        }
    }
    processor->input_queue = PLUTO_CreateMessageQueue(
        config->base_path,
        config->name_of_input_queue,
        PLUTO_PROC_INPUT_QUEUE_PERMISSIONS,
        logger
    ); 
    
    processor->system_event_handler = PLUTO_CreateSystemEventHandler(processor->logger);
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
    PLUTO_Free((*processor)->output_queues);
    PLUTO_DestroySystemEventHandler(&(*processor)->system_event_handler);
    PLUTO_Free(*processor);
}

bool PLUTO_ProcessorEmitEvent(PLUTO_Processor_t processor, PLUTO_Event_t event)
{
    struct PLUTO_MsgBuf buffer;
    PLUTO_EventToBuffer(
        event,
        buffer.text,
        sizeof(buffer.text)
    );
    return PLUTO_MessageQueueWrite(
        processor->input_queue,
        &buffer
    ); 
}

static void PLUTO_ProcessorExecuteCallback(PLUTO_Processor_t processor, struct PLUTO_Event *event);
static void PLUTO_ProcessorDispatchSignalEvents(PLUTO_Processor_t processor);
static void PLUTO_ProcessorDispatchSystemEvents(PLUTO_Processor_t processor);
static bool PLUTO_ProcessorDispatchExternalEvents(PLUTO_Processor_t processor);

bool PLUTO_ProcessorProcess(PLUTO_Processor_t processor)
{
    //
    // Get System Events.
    //
    PLUTO_ProcessorDispatchSystemEvents(processor);
    //
    // Get Signal Events...
    //
    PLUTO_ProcessorDispatchSignalEvents(processor);
    //
    // Get Events from Queue...
    //
    (void)PLUTO_ProcessorDispatchExternalEvents(processor);
    return false;
}

static void PLUTO_ProcessorExecuteCallback(PLUTO_Processor_t processor, struct PLUTO_Event *event)
{
    struct PLUTO_MsgBuf buffer;
    //memset(buffer.text, '\0', sizeof(buffer.text));
    //PLUTO_Event_t output_event = PLUTO_CreateEvent();
    PLUTO_Event_t output_event = &processor->output_event; 
    memset(PLUTO_EventPayload(output_event), '\0', PLUTO_EventSizeOfPayloadBuffer(output_event));
    
    // process...
    PLUTO_ProcessorCallbackInput_t input = {
        .id = event->eventid,
        .event = PLUTO_EventId(event),
        .input_buffer = PLUTO_EventPayload(event),
        .output_buffer = PLUTO_EventPayload(output_event),
        .input_buffer_size = PLUTO_EventSizeOfPayload(event),
        .output_buffer_size = PLUTO_EventSizeOfPayloadBuffer(output_event),
        .number_of_output_queues = (uint8_t)processor->number_of_output_queues
    };
    PLUTO_ProcessorCallbackOutput_t output = processor->callback(&input);
    PLUTO_EventSetTimestamp(output_event, PLUTO_TimeNow());
    PLUTO_EventSetSizeOfPayload(output_event, output.output_size);
    PLUTO_EventSetEvent(output_event, output.event);
    PLUTO_EventSetId(output_event, output.id);
    //
    // Only send an Event if the Client returned True.
    //
    if(output.return_value)
    {
        buffer.text[output.output_size] = '\0';
        if(PLUTO_EventToBuffer(output_event, buffer.text, sizeof(buffer.text) - 1))
        {
            for(int i=0;i<processor->number_of_output_queues;++i)
            {
                if((1LU << i) & output.output_to_queues)
                {
                    PLUTO_LoggerDebug(processor->logger, "Send Event to Outputqueue %i...", i);
                    PLUTO_MessageQueueWrite(
                        processor->output_queues[i],
                        &buffer
                    ); 
                }
            }
        } 
        else
        {
            printf("Event to Buffer failed!\n");
        }
    }
    //PLUTO_DestroyEvent(&output_event);
}

static void PLUTO_ProcessorDispatchSignalEvents(PLUTO_Processor_t processor)
{
    char buffer[128];
    PLUTO_SignalEvent_t signal_event = {0};
    while(PLUTO_SignalPendingEvent(processor->signal_handler, &signal_event))
    {
        PLUTO_TimeToString(signal_event.timestamp, buffer, sizeof(buffer));
        
        //PLUTO_Event_t tmp = PLUTO_CreateEvent();
        PLUTO_Event_t tmp = &processor->event_buffer;
        PLUTO_EventSetEvent(tmp, PLUTO_NODE_EVENT_DEFINITION_SIGNAL_EVENT);
        PLUTO_EventSetTimestamp(tmp, signal_event.timestamp);
        PLUTO_EventSetId(tmp, 0);
        const int len = snprintf(
            PLUTO_EventPayload(tmp),
            PLUTO_EventSizeOfPayloadBuffer(tmp),
            "{\"signal\":%i}", 
            signal_event.signum
        );
        PLUTO_EventSetSizeOfPayload(tmp, len);
        PLUTO_ProcessorExecuteCallback(processor, tmp);
        //PLUTO_DestroyEvent(&tmp);
    }
}

static void PLUTO_ProcessorDispatchSystemEvents(PLUTO_Processor_t processor)
{
    PLUTO_SystemEvent_t sevent = &processor->system_event;//PLUTO_CreateSystemEvent();
    int count = 10;
    while(count--)
    {
        if(PLUTO_SE_OK != PLUTO_SystemEventsPoll(processor->system_event_handler, sevent))
            break;
        //PLUTO_Event_t tmp = PLUTO_CreateEvent();
        PLUTO_Event_t tmp = &processor->event_buffer;
        PLUTO_EventSetEvent(tmp, PLUTO_NODE_EVENT_DEFINITION_SYSTEM_EVENT);
        PLUTO_EventSetTimestamp(tmp, sevent->timestamp);
        PLUTO_EventSetId(tmp, 0);
        const int len = snprintf(
            PLUTO_EventPayload(tmp),
            PLUTO_EventSizeOfPayloadBuffer(tmp),
            "{\"file-descriptor\":%i}", 
            PLUTO_SystemEventFiledescriptor(sevent)
        );
        PLUTO_EventSetSizeOfPayload(tmp, len);
        PLUTO_ProcessorExecuteCallback(processor, tmp);
        //PLUTO_DestroyEvent(&tmp);
    }
    //PLUTO_DestroySystemEvent(&sevent);
}

static bool PLUTO_ProcessorDispatchExternalEvents(PLUTO_Processor_t processor)
{
    struct PLUTO_MsgBuf buffer;
    //memset(buffer.text, '\0', sizeof(buffer.text));
    if(
        PLUTO_MessageQueueRead(
            processor->input_queue,
            &buffer
        )
    )
    {
        //PLUTO_Event_t event = PLUTO_CreateEvent();
        memset(PLUTO_EventPayload(&processor->event_buffer), '\0', PLUTO_EventSizeOfPayloadBuffer(&processor->event_buffer));
        bool result = PLUTO_CreateEventFromBuffer(&processor->event_buffer, buffer.text, sizeof(buffer.text));
        if(result)
        {    
            PLUTO_ProcessorExecuteCallback(processor, &processor->event_buffer);
            //PLUTO_DestroyEvent(&event);
            return true;
        }
        //PLUTO_DestroyEvent(&event);
    }
    return false;
}

//
// --------------------------------------------------------------------------------------------------------------------
//
