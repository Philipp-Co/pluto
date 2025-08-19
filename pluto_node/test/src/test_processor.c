
#include <pluto/pluto_edge/pluto_edge.h>
#include <pluto/pluto_node/test/test_processor.h>
#include <pluto/application_layer/pluto_processor.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/signals/pluto_signal.h>
#include <pluto/pluto_node/test/prepare.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <Unity/src/unity.h>

#include <string.h>


PLUTO_ProcessorCallbackOutput_t PLUTO_TEST_SimpleCallback(PLUTO_ProcessorCallbackInput_t *args);

void PLUTO_TEST_ProcessorInitial(void)
{
    PLUTO_ProcessCallback_t callback = PLUTO_TEST_SimpleCallback;
    PLUTO_SignalHandler_t signal_handler = PLUTO_CreateSignalHandler(PLUTO_TEST_processor_logger);
    PLUTO_Processor_t processor = PLUTO_CreateProcessor(
        PLUTO_TEST_config,
        signal_handler,
        callback,
        PLUTO_TEST_processor_logger
    );
    PLUTO_DestroyProcessor(&processor);
    PLUTO_DestroySignal(&signal_handler);
}

void PLUTO_TEST_ProcessorProcessWithEmptyPayload(void)
{
    PLUTO_ProcessCallback_t callback = PLUTO_TEST_SimpleCallback;
    PLUTO_Processor_t processor = PLUTO_CreateProcessor(
        PLUTO_TEST_config,
        PLUTO_CreateSignalHandler(PLUTO_TEST_processor_logger),
        callback,
        PLUTO_TEST_processor_logger
    );
    
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(
        PLUTO_TEST_config->base_path,
        PLUTO_TEST_name,
        0,
        PLUTO_TEST_edge_logger
    ); 
    TEST_ASSERT_NOT_NULL(edge);

    PLUTO_Event_t event = PLUTO_CreateEvent();
    const bool result = PLUTO_EDGE_EdgeSendEvent(edge, event);

    PLUTO_ProcessorProcess(processor);
    
    PLUTO_DestroyEvent(&event);
    PLUTO_EDGE_DestroyEdge(&edge);
    PLUTO_DestroyProcessor(&processor);
    TEST_ASSERT_TRUE(
        result
    );
}

void PLUTO_TEST_ProcessorProcessWithStandardPayload(void)
{
    PLUTO_ProcessCallback_t callback = PLUTO_TEST_SimpleCallback;
    PLUTO_SignalHandler_t signal_handler = PLUTO_CreateSignalHandler(PLUTO_TEST_processor_logger);
    PLUTO_Processor_t processor = PLUTO_CreateProcessor(
        PLUTO_TEST_config,
        signal_handler,
        callback,
        PLUTO_TEST_processor_logger
    );
    printf("TEST 1\n");
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(
        PLUTO_TEST_config->base_path,
        PLUTO_TEST_name,
        0,
        PLUTO_TEST_edge_logger
    ); 
    TEST_ASSERT_NOT_NULL(edge);

    printf("TEST 2\n");
    PLUTO_Event_t event = PLUTO_CreateEvent();
    snprintf(
        PLUTO_EventPayload(event), 
        PLUTO_EventSizeOfPayload(event),
        "ThisIsATest"    
    );
    printf("TEST 3\n");
    PLUTO_EventSetSizeOfPayload(event, strlen("ThisIsATest"));
    const bool result = PLUTO_EDGE_EdgeSendEvent(edge, event);

    printf("TEST 4\n");
    PLUTO_ProcessorProcess(processor);

    printf("TEST 5\n");
    PLUTO_DestroyEvent(&event);
    PLUTO_EDGE_DestroyEdge(&edge);
    PLUTO_DestroyProcessor(&processor);

    TEST_ASSERT_TRUE(
        result
    );
}

void PLUTO_TEST_ProcessorProcessWithBigPayload(void)
{
    PLUTO_ProcessCallback_t callback = PLUTO_TEST_SimpleCallback;
    PLUTO_Processor_t processor = PLUTO_CreateProcessor(
        PLUTO_TEST_config,
        PLUTO_CreateSignalHandler(PLUTO_TEST_processor_logger),
        callback,
        PLUTO_TEST_processor_logger
    );
    
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(
        PLUTO_TEST_config->base_path,
        PLUTO_TEST_name,
        0,
        PLUTO_TEST_edge_logger
    ); 
    TEST_ASSERT_NOT_NULL(edge);

    char buffer[1024];
    memset(buffer, 'a', sizeof(buffer));
    buffer[1023] = '\0';

    PLUTO_Event_t event = PLUTO_CreateEvent();
    TEST_ASSERT_NOT_NULL(event);
    snprintf(
        PLUTO_EventPayload(event), 
        PLUTO_EventSizeOfPayloadBuffer(event),
        "%s",
        buffer
    );
    PLUTO_EventSetSizeOfPayload(event, strlen(buffer));
    const bool result = PLUTO_EDGE_EdgeSendEvent(edge, event);
    PLUTO_ProcessorProcess(processor);
    
    PLUTO_DestroyEvent(&event);
    PLUTO_EDGE_DestroyEdge(&edge);
    PLUTO_DestroyProcessor(&processor);
    
    TEST_ASSERT_FALSE(
        result
    );
}

void PLUTO_TEST_ProcessorProcessWithMultipleMessages(void)
{
    PLUTO_ProcessCallback_t callback = PLUTO_TEST_SimpleCallback;
    PLUTO_Processor_t processor = PLUTO_CreateProcessor(
        PLUTO_TEST_config,
        PLUTO_CreateSignalHandler(PLUTO_TEST_processor_logger),
        callback,
        PLUTO_TEST_processor_logger
    );
    
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(
        PLUTO_TEST_config->base_path,
        PLUTO_TEST_name,
        0,
        PLUTO_TEST_edge_logger
    ); 
    TEST_ASSERT_NOT_NULL(edge);

    static const char *buffer[5] = 
    {
        "",
        "aaa",
        "",
        "ababaa",
        "zz"
    };
    
    int result = 0;
    for(int32_t i=0;i<5;++i)
    {
        PLUTO_Event_t event = PLUTO_CreateEvent();
        TEST_ASSERT_NOT_NULL(event);
        snprintf(
            PLUTO_EventPayload(event), 
            PLUTO_EventSizeOfPayloadBuffer(event),
            "%s",
            buffer[i]
        );
        PLUTO_EventSetSizeOfPayload(event, strlen(buffer[i]));
        result += (int)PLUTO_EDGE_EdgeSendEvent(edge, event);
        PLUTO_ProcessorProcess(processor);
        PLUTO_DestroyEvent(&event);
    }

    PLUTO_EDGE_DestroyEdge(&edge);
    PLUTO_DestroyProcessor(&processor);

    TEST_ASSERT_EQUAL_INT(
        5,
        result
    );
}

PLUTO_ProcessorCallbackOutput_t PLUTO_TEST_SimpleCallback(PLUTO_ProcessorCallbackInput_t *args)
{
    const int buffer_size = args->input_buffer_size > args->output_buffer_size ? args->output_buffer_size : args->input_buffer_size;
    memset(args->output_buffer, '\0', args->output_buffer_size);
    memcpy(args->output_buffer, args->input_buffer, buffer_size);

    PLUTO_ProcessorCallbackOutput_t output = {
        .id=args->id,
        .event=args->event,
        .output_size=0,
        .output_to_queues = 0xFFFFFFFFFFFFFFFFU,
        .return_value = true
    };
    return output;
}
