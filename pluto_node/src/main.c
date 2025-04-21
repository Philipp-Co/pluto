
#include <pluto/pluto_compile_time_switches.h>
#include "pluto/pluto_config.h"
#include <pluto/pluto_processor.h>

#if PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_RTM_PYTHON
#include <pluto/python/pluto_python.h>
#endif

#include <string.h>
#include <signal.h>
#include <stdatomic.h>
#include <unistd.h>
#include <stdlib.h>


typedef struct
{
    char name[256];
    char config_path[4096];
} PLUTO_Arguments_t;

void PLUTO_SignalHandler(int signum);
static bool PLUTO_ParseArguments(PLUTO_Arguments_t *args, int argc, char **argv);

#if PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_PASSTHROUGHT
static PLUTO_ProcessorCallbackOutput_t PLUTO_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args);
#endif

static atomic_int PLUTO_Terminate;

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    atomic_store(&PLUTO_Terminate, 0);
    signal(SIGINT, PLUTO_SignalHandler);
  
    PLUTO_Arguments_t *args = malloc(sizeof(PLUTO_Arguments_t));
    if(!PLUTO_ParseArguments(args, argc, argv))
    {
        free(args);
        return -1;
    }

#if PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_RTM_PYTHON
    // Initialize Python.
    if(!PLUTO_InitializePython())
    {
        return -1;
    }
#elif PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_RTM_SHARED_LIB
    // Initialize Shared Library.
#endif

    PLUTO_Config_t config = PLUTO_CreateConfig(args->config_path, args->name);
    if(!config)
    {
        free(args);
        printf("Unable to read Config from file.\n");
        return -1;
    }
    free(args);
    
    printf("Run main Program...\n");
    PLUTO_Processor_t processor = PLUTO_CreateProcessor(
        config,
#if PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_PASSTHROUGHT
        PLUTO_ProcessCallback
#elif PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_RTM_SHARED_LIB
#error "Not Implemented!"
#elif PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_RTM_PYTHON
        PLUTO_PY_ProcessCallback
#else
#error "Unknown Runtime Mode!"
#endif
    );
    PLUTO_DestroyConfig(&config);
    if(!processor)
    {
        return -1;
    }
    while(!atomic_load(&PLUTO_Terminate))
    {
        PLUTO_ProcessorProcess(processor);
    }
    PLUTO_DestroyProcessor(&processor);


#if PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_RTM_PYTHON
    // Deinitialize Python.
    PLUTO_DeinitializePython();
#elif PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_RTM_SHARED_LIB
    // Deinitialize Shared Library.
#endif
    
    printf("Bye Bye...\n");
    return 0;
}

void PLUTO_SignalHandler(int signum)
{
    (void)signum;
    atomic_store(&PLUTO_Terminate, 1);
}

static bool PLUTO_ParseArguments(PLUTO_Arguments_t *args, int argc, char **argv)
{
    memset(args->name, '\0', sizeof(args->name));
    memset(args->config_path, '\0', sizeof(args->config_path));

    char c;
    while((c = getopt(argc, argv, "n:c:")))
    {
        if(((char)-1) == (char)c)
        {
            break;
        }

        switch(c)
        {
            case 'n':
                memcpy(args->name, optarg, strlen(optarg));
                break;
            case 'c':
                memcpy(args->config_path, optarg, strlen(optarg));
                break;
            default:
                printf("Unknown Argument 0x%x\n", (int)c);
                return false;
        }
    }
    return true;
}

#if PLUTO_CTS_RUNTIME_MODE == PLUTO_CTS_PASSTHROUGHT
static PLUTO_ProcessorCallbackOutput_t PLUTO_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args)
{
    printf(
        "Process:\n"
        "  Request: %s\n",
        args->input_buffer
    );
    memcpy(args->output_buffer, args->input_buffer, args->input_buffer_size);
    PLUTO_ProcessorCallbackOutput_t output = {
        .return_value = true,
        .output_size = args->input_buffer_size
    };
    return output;
}
#endif
