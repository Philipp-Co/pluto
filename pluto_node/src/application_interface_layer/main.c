//
// --------------------------------------------------------------------------------------------------------------------
//
#include <pluto/application_layer/pluto_compile_time_switches.h>
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/application_layer/pluto_processor.h>
#include <pluto/os_abstraction/pluto_malloc.h>
#include <pluto/os_abstraction/pluto_logger.h>

#if defined(PLUTO_CTS_RTM_PYTHON)
#include <pluto/application_layer/python/pluto_python.h>
#elif defined(PLUTO_CTS_RTM_SHARED_LIB)
#include <pluto/application_layer/shared_library/pluto_shared_library.h>
#endif

#include <string.h>
#include <signal.h>
#include <stdatomic.h>
#include <unistd.h>
#include <stdlib.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

typedef struct
{
    char name[256];
    char config_path[4096];
    char executable[4096];
#if defined(PLUTO_CTS_RTM_PYTHON)
    char python_path[8192];
#endif
} PLUTO_Arguments_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_SignalHandler(int signum);
static bool PLUTO_ParseArguments(PLUTO_Arguments_t *args, int argc, char **argv);

#if defined(PLUTO_CTS_RTM_PASSTHROUGH)
static PLUTO_ProcessorCallbackOutput_t PLUTO_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args);
#endif

//
// --------------------------------------------------------------------------------------------------------------------
//

static atomic_int PLUTO_Terminate;
static PLUTO_Logger_t PLUTO_node_logger = NULL;

//
// --------------------------------------------------------------------------------------------------------------------
//

int main(int argc, char **argv)
{
    int return_value = -1;
    atomic_store(&PLUTO_Terminate, 0);
    signal(SIGINT, PLUTO_SignalHandler);
    printf("Hello World: %s!\n", argv[0]);
  
    PLUTO_Arguments_t *args = PLUTO_Malloc(sizeof(PLUTO_Arguments_t));
    if(!args)
    {
        printf("Uanble to allocate Memory for Arugments...\n");
        goto end;
    } 
    memset(args, '\0', sizeof(*args));
    if(!PLUTO_ParseArguments(args, argc, argv))
    {
        printf("Unable to parse Arguments!\n");
        PLUTO_Free(args);
        goto end;
    }
    PLUTO_node_logger = PLUTO_CreateLogger(args->name);

#if defined(PLUTO_CTS_RTM_PYTHON)
    // Initialize Python.
    if(!PLUTO_InitializePython(args->python_path, args->executable, PLUTO_node_logger))
    {
        PLUTO_LoggerError(PLUTO_node_logger, "Unable to initialize Python.");
        goto end;
    }
    PLUTO_LoggerInfo(PLUTO_node_logger, "Python successfully initialized.");
#elif defined(PLUTO_CTS_RTM_SHARED_LIB)
    // Initialize Shared Library.
    PLUTO_SHLIB_Initialize(args->executable);
#endif

    PLUTO_Config_t config = PLUTO_CreateConfig(args->config_path, args->name, PLUTO_node_logger);
    if(!config)
    {
        PLUTO_LoggerError(PLUTO_node_logger, "Unable to read Config from file %s.", args->config_path);
        PLUTO_Free(args);
        goto end;
    }
    PLUTO_Free(args);
    
    PLUTO_LoggerInfo(PLUTO_node_logger, "Run main Program...");
    PLUTO_Processor_t processor = PLUTO_CreateProcessor(
        config,
#if defined(PLUTO_CTS_RTM_PASSTHROUGH)
        PLUTO_ProcessCallback,
#elif defined(PLUTO_CTS_RTM_SHARED_LIB)
        PLUTO_SHLIB_ProcessCallback,
#elif defined(PLUTO_CTS_RTM_PYTHON)
        PLUTO_PY_ProcessCallback,
#else
#error "Unknown Runtime Mode!"
#endif
        PLUTO_node_logger
    );
    PLUTO_DestroyConfig(&config);
    if(!processor)
    {
        PLUTO_LoggerError(PLUTO_node_logger, "Unable to create a Processor.");
        goto end;
    }
    while(!atomic_load(&PLUTO_Terminate))
    {
        PLUTO_ProcessorProcess(processor);
        sleep(1);
    }
    PLUTO_DestroyProcessor(&processor);

#if defined(PLUTO_CTS_RTM_PYTHON)
    // Deinitialize Python.
    PLUTO_DeinitializePython();
#elif defined(PLUTO_CTS_RTM_SHARED_LIB)
    // Deinitialize Shared Library.
    PLUTO_SHLIB_Destroy();
#endif
    
    return_value = 0;
end:
    PLUTO_LoggerInfo(PLUTO_node_logger, "Node: Bye Bye...");
    PLUTO_DestroyLogger(&PLUTO_node_logger);
    return return_value;
}

//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_SignalHandler(int signum)
{
    (void)signum;
    atomic_store(&PLUTO_Terminate, 1);
}

static bool PLUTO_ParseArguments(PLUTO_Arguments_t *args, int argc, char **argv)
{
    memset(args->name, '\0', sizeof(args->name));
    memset(args->config_path, '\0', sizeof(args->config_path));
    memset(args->executable, '\0', sizeof(args->executable));
#if defined(PLUTO_CTS_RTM_PYTHON)
    memset(args->python_path, '\0', sizeof(args->python_path));
#endif

    char c;
#if defined(PLUTO_CTS_RTM_PYTHON)
    static const char *optstring = "n:c:e:p:";
#elif defined(PLUTO_CTS_RTM_SHARED_LIB)
    static const char *optstring = "n:c:e:";
#else
    static const char *optstring = "n:c:";
#endif
    while((c = getopt(argc, argv, optstring)) != -1)
    {
        if(c == ((char)-1))
        {
            break;
        }
        switch(c)
        {
#if defined(PLUTO_CTS_RTM_PYTHON)
            case 'p':
                memcpy(args->python_path, optarg, strlen(optarg));
                break;
#endif
#if defined(PLUTO_CTS_RTM_PYTHON) || defined(PLUTO_CTS_RTM_SHARED_LIB)
            case 'e':
                memcpy(args->executable, optarg, strlen(optarg));
                break;
#endif
            case 'n':
                memcpy(args->name, optarg, strlen(optarg));
                break;
            case 'c':
                memcpy(args->config_path, optarg, strlen(optarg));
                break;
            default:
                return false;
        }
    }

#if defined(PLUTO_CTS_RTM_PYTHON)
    if(strlen(args->python_path) == 0)
    {
        return false;
    }
#endif

#if defined(PLUTO_CTS_RTM_PYTHON) || defined(PLUTO_CTS_RTM_SHARED_LIB)
    if(strlen(args->executable) == 0)
    {
        return false;
    }
#endif
    return true;
}

#if defined(PLUTO_CTS_RTM_PASSTHROUGH)
static PLUTO_ProcessorCallbackOutput_t PLUTO_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args)
{
    snprintf(args->output_buffer, args->output_buffer_size, "{\"passthrough\":\"%s\"}", args->input_buffer);
    PLUTO_ProcessorCallbackOutput_t output = {
        .id = args->id,
        .output_to_queues = 0xFFFFFFFFFFFFFFFFU,
        .event = args->event,
        .return_value = true,
        .output_size = strlen(args->output_buffer)
    };
    return output;
}
#endif

//
// --------------------------------------------------------------------------------------------------------------------
//
