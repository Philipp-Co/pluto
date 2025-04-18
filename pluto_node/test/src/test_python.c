#include "Unity/src/unity.h"
#include <pluto/application_layer/pluto_processor.h>
#include <pluto/application_layer/python/pluto_python.h>
#include <pluto/pluto_node/test/prepare.h>

static PLUTO_Processor_t PLUTO_TEST_PythonCreateProcessor(void);
static void PLUTO_TEST_PythonDestroyProcessor(PLUTO_Processor_t processor);

void PLUTO_TEST_PythonInitial(void)
{
    PLUTO_Processor_t processor = PLUTO_TEST_PythonCreateProcessor();
    PLUTO_TEST_PythonDestroyProcessor(processor);
}

void PLUTO_TEST_PythonProcessEventWhenQueueIsEmpty(void)
{
    PLUTO_Processor_t processor = PLUTO_TEST_PythonCreateProcessor();
    PLUTO_ProcessorProcess(processor);
    PLUTO_TEST_PythonDestroyProcessor(processor);
}

void PLUTO_TEST_PythonProcessEventWhenQueueIsNotEmpty(void)
{
    PLUTO_Processor_t processor = PLUTO_TEST_PythonCreateProcessor();
    PLUTO_EDGE_Edge_t node_input_edge = PLUTO_EDGE_CreateEdge(
        PLUTO_TEST_config->base_path,
        PLUTO_TEST_name,
        0,
        PLUTO_TEST_edge_logger
    ); 
    PLUTO_EDGE_Edge_t node_output_edge = PLUTO_EDGE_CreateEdge(
        PLUTO_TEST_config->base_path,
        "pluto-0_oq_0",
        0,
        PLUTO_TEST_edge_logger
    ); 
    //
    // Test begins here...
    //
    int32_t sucessfully_send_events = 0;
    for(int32_t i=0;i<3;++i)
    {
        PLUTO_Event_t event = PLUTO_CreateEvent();
        const bool result = PLUTO_EDGE_EdgeSendEvent(node_input_edge, event);
        sucessfully_send_events += (int32_t)result;
        PLUTO_DestroyEvent(&event);
    }

    int32_t sucessfully_read_events = 0;
    for(int32_t i=0;i<3;++i)
    {
        PLUTO_ProcessorProcess(processor);
        PLUTO_Event_t event = PLUTO_CreateEvent();
        sucessfully_read_events += (int32_t) PLUTO_EDGE_EdgeReceiveEvent(node_output_edge, event);
        PLUTO_DestroyEvent(&event);
    }

    PLUTO_EDGE_DestroyEdge(&node_input_edge);
    PLUTO_EDGE_DestroyEdge(&node_output_edge);
    PLUTO_TEST_PythonDestroyProcessor(processor);

    //
    // Run Assertions...
    //
    TEST_ASSERT_EQUAL_INT32(3, sucessfully_send_events);
    TEST_ASSERT_EQUAL_INT32(3, sucessfully_read_events);
}


PLUTO_Processor_t PLUTO_TEST_PythonCreateProcessor(void)
{
    PLUTO_InitializePython(
        PLUTO_TEST_python_path,
        PLUTO_TEST_executable,
        PLUTO_TEST_processor_logger 
    );
    PLUTO_ProcessCallback_t callback = PLUTO_PY_ProcessCallback;
    PLUTO_Processor_t processor = PLUTO_CreateProcessor(
        PLUTO_TEST_config,
        PLUTO_CreateSignalHandler(PLUTO_TEST_processor_logger),
        callback,
        PLUTO_TEST_processor_logger
    );
    return processor;
}

void PLUTO_TEST_PythonDestroyProcessor(PLUTO_Processor_t processor)
{
    PLUTO_DestroyProcessor(&processor);
    PLUTO_DeinitializePython();
}

