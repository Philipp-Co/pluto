///
/// \brief  Implements a Processor. The Processor is the main Structure which drives the Application.
///
#ifndef __PLUTO_PROCESSOR_H__
#define __PLUTO_PROCESSOR_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/application_layer/pluto_info.h>
#include <pluto/types/pluto_types.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

typedef PLUTO_ProcessorCallbackOutput_t (*PLUTO_ProcessCallback_t)(PLUTO_ProcessorCallbackInput_t *args);

struct PLUTO_Processor
{
    PLUTO_Logger_t logger;
    PLUTO_ProcessCallback_t callback;
    PLUTO_Info_t info;
    PLUTO_MessageQueue_t input_queue;
    int32_t number_of_output_queues;
    PLUTO_MessageQueue_t *output_queues;
}; 
typedef struct PLUTO_Processor* PLUTO_Processor_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Create a Processor.
///
PLUTO_Processor_t PLUTO_CreateProcessor(PLUTO_Config_t config, PLUTO_ProcessCallback_t callback, PLUTO_Logger_t logger);
///
/// \brief  Destroy a Processor.
///
void PLUTO_DestroyProcessor(PLUTO_Processor_t *processor);
///
/// \brief  Read the next input Event, if there is one, pass it to the Applicationlogic and output the Result as the next
///         output Event.
///
void PLUTO_ProcessorProcess(PLUTO_Processor_t processor);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
