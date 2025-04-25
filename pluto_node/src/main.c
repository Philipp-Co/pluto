//
// --------------------------------------------------------------------------------------------------------------------
//
#include <pluto/pluto_compile_time_switches.h>
#include "pluto/pluto_config.h"
#include <pluto/pluto_processor.h>

#if defined(PLUTO_CTS_RTM_PYTHON)
#include <pluto/python/pluto_python.h>
#elif defined(PLUTO_CTS_RTM_SHARED_LIB)
#include <pluto/shared_library/pluto_shared_library.h>
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

//
// --------------------------------------------------------------------------------------------------------------------
//

int main(int argc, char **argv)
{
    printf("Hello World!\n");
    atomic_store(&PLUTO_Terminate, 0);
    signal(SIGINT, PLUTO_SignalHandler);
  
    PLUTO_Arguments_t *args = malloc(sizeof(PLUTO_Arguments_t));
    if(!args)
    {
        return -1;
    } 
    memset(args, '\0', sizeof(*args));
    if(!PLUTO_ParseArguments(args, argc, argv))
    {
        free(args);
        return -1;
    }

#if defined(PLUTO_CTS_RTM_PYTHON)
    // Initialize Python.
    if(!PLUTO_InitializePython(args->python_path, args->executable))
    {
        printf("Unable to Initialize Python.\n");
        return -1;
    }
    printf("Python Initialization done.\n");
#elif defined(PLUTO_CTS_RTM_SHARED_LIB)
    // Initialize Shared Library.
    PLUTO_SHLIB_Initialize(args->executable);
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
#if defined(PLUTO_CTS_RTM_PASSTHROUGH)
        PLUTO_ProcessCallback
#elif defined(PLUTO_CTS_RTM_SHARED_LIB)
        PLUTO_SHLIB_ProcessCallback
#elif defined(PLUTO_CTS_RTM_PYTHON)
        PLUTO_PY_ProcessCallback
#else
#error "Unknown Runtime Mode!"
#endif
    );
    PLUTO_DestroyConfig(&config);
    if(!processor)
    {
        printf("Unable to create a Processor.\n");
        return -1;
    }
    while(!atomic_load(&PLUTO_Terminate))
    {
        PLUTO_ProcessorProcess(processor);
    }
    PLUTO_DestroyProcessor(&processor);


#if defined(PLUTO_CTS_RTM_PYTHON)
    // Deinitialize Python.
    PLUTO_DeinitializePython();
#elif defined(PLUTO_CTS_RTM_SHARED_LIB)
    // Deinitialize Shared Library.
    PLUTO_SHLIB_Destroy();
#endif
    
    printf("Bye Bye...\n");
    return 0;
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
    while((c = getopt(argc, argv, optstring)))
    {
        if(((char)-1) == (char)c)
        {
            break;
        }

        switch(c)
        {
#if defined(PLUTO_CTS_RTM_PYTHON)
            case 'p':
                printf("-p %s\n", optarg);
                memcpy(args->python_path, optarg, strlen(optarg));
                break;
#endif
#if defined(PLUTO_CTS_RTM_PYTHON) || defined(PLUTO_CTS_RTM_SHARED_LIB)
            case 'e':
                printf("-e %s\n", optarg);
                memcpy(args->executable, optarg, strlen(optarg));
                break;
#endif
            case 'n':
                printf("-n %s\n", optarg);
                memcpy(args->name, optarg, strlen(optarg));
                break;
            case 'c':
                printf("-c %s\n", optarg);
                memcpy(args->config_path, optarg, strlen(optarg));
                break;
            default:
                printf("Unknown Argument 0x%x\n", (int)c);
                return false;
        }
    }

#if defined(PLUTO_CTS_RTM_PYTHON)
    if(strlen(args->python_path) == 0)
    {
        printf("No Python Path given!\n");
        return false;
    }
#endif

#if defined(PLUTO_CTS_RTM_PYTHON) || defined(PLUTO_CTS_RTM_SHARED_LIB)
    if(strlen(args->executable) == 0)
    {
        printf("No Executable given!\n");
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
        .return_value = true,
        .output_size = strlen(args->output_buffer)
    };
    return output;
}
#endif

//
// --------------------------------------------------------------------------------------------------------------------
//
