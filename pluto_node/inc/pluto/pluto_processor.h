///
/// \brief  Implements a Processor. The Processor is the main Structure which drives the Application.
///
#ifndef __PLUTO_PROCESSOR_H__
#define __PLUTO_PROCESSOR_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_config.h>
#include <pluto/pluto_message_queue.h>
#include <pluto/pluto_info.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

typedef struct 
{
      char *input_buffer;
      char *output_buffer;
      size_t input_buffer_size;
      size_t output_buffer_size;
} PLUTO_ProcessorCallbackInput_t;

typedef struct
{
    size_t output_size;
    bool return_value;
} PLUTO_ProcessorCallbackOutput_t;

typedef PLUTO_ProcessorCallbackOutput_t (*PLUTO_ProcessCallback_t)(PLUTO_ProcessorCallbackInput_t *args);

struct PLUTO_Processor
{
    PLUTO_ProcessCallback_t callback;
    PLUTO_Info_t info;
    PLUTO_MessageQueue_t input_queue;
    int32_t number_of_output_queues;
    PLUTO_MessageQueue_t *output_queues;
    PLUTO_Request_t request;
}; 
typedef struct PLUTO_Processor* PLUTO_Processor_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Create a Processor.
///
PLUTO_Processor_t PLUTO_CreateProcessor(PLUTO_Config_t config, PLUTO_ProcessCallback_t callback);
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
