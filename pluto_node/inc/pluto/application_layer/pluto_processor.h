///
/// \brief  Implements a Processor. The Processor is the main Structure which drives the Application.
///
#ifndef __PLUTO_PROCESSOR_H__
#define __PLUTO_PROCESSOR_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include "pluto/os_abstraction/files/pluto_file.h"
#include "pluto/pluto_event/pluto_event.h"
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/application_layer/pluto_info.h>
#include <pluto/os_abstraction/signals/pluto_signal.h>
#include <pluto/os_abstraction/system_events/pluto_system_events.h>
#include <pluto/types/pluto_types.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

typedef PLUTO_ProcessorCallbackOutput_t (*PLUTO_ProcessCallback_t)(PLUTO_ProcessorCallbackInput_t *args);

struct PLUTO_Processor
{
    PLUTO_Logger_t logger;
    PLUTO_SignalHandler_t signal_handler;
    PLUTO_ProcessCallback_t callback;
    PLUTO_Info_t info;
    PLUTO_MessageQueue_t input_queue;
    int32_t number_of_output_queues;
    PLUTO_MessageQueue_t *output_queues;

    PLUTO_SystemEventHandler_t system_event_handler;
}; 
typedef struct PLUTO_Processor* PLUTO_Processor_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Create a Processor.
///
PLUTO_Processor_t PLUTO_CreateProcessor(
    PLUTO_Config_t config, 
    PLUTO_SignalHandler_t signal_handler,
    PLUTO_ProcessCallback_t callback, 
    PLUTO_Logger_t logger
);
///
/// \brief  Destroy a Processor.
///
void PLUTO_DestroyProcessor(PLUTO_Processor_t *processor);
///
/// \brief  Read the next input Event, if there is one, pass it to the Applicationlogic and output the Result as the next
///         output Event.
///
bool PLUTO_ProcessorProcess(PLUTO_Processor_t processor);
///
/// \brief  Emit an Event.
///         The Event is written to the Inputqueue of the Processor.
///         Be careful with this Function: 
///         If to many Events are emitted the Inputqueue can overflow very fast!!!
///
/// \returns bool - true if the Event was emitted successfully, false otherwise.
///
bool PLUTO_ProcessorEmitEvent(PLUTO_Processor_t processor, PLUTO_Event_t event);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
