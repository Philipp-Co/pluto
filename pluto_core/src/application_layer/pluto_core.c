//
// --------------------------------------------------------------------------------------------------------------------
//
#include "pluto/os_abstraction/pluto_logger.h"
#include "pluto/pluto_core/application_layer/pluto_core_state.h"
#include "pluto/pluto_core/application_layer/pluto_node_state.h"
#include "pluto/pluto_core/data_layer/pluto_core_register.h"
#include "pluto/pluto_core/data_layer/pluto_sig_queue.h"
#include <pluto/pluto_core/application_layer/pluto_core.h>

#include <pluto/os_abstraction/pluto_malloc.h>

#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

//
// What can happen?
//  - SIGINT is received -> kill all Subprocesses -> Wait for all Subprocesses to terminate -> Terminate yourselve
//

///
/// \brief  Getter for the Directory where PLUTOs Executables are stored.
///
static const char* PLUTO_GetBinaryDirectory(PLUTO_Logger_t logger);

//
// --------------------------------------------------------------------------------------------------------------------
//
PLUTO_Core_t PLUTO_CreateCore(const char *filename, PLUTO_Logger_t logger)
{
    //char *binary_dir = getenv("PLUTO_BINARY_DIR");
    const char* binary_dir = PLUTO_GetBinaryDirectory(logger);
    if(!binary_dir)
    {
        PLUTO_LoggerError(logger, "Error: PLUTO_BINARY_DIR ist not set!");
        return NULL;
    }

    PLUTO_CoreConfig_t config = PLUTO_CreateCoreConfig(filename, logger);
    if(!config)
    {
        PLUTO_LoggerError(logger, "Unable to create Core Object because the Program was unable to read the given Config.\n");
        return NULL;
    }
    
    const size_t n_nodes = PLUTO_CoreConfigNumberOfNodes(config);
    PLUTO_LoggerInfo(
        logger,
        "Starting %i Nodes...", n_nodes
    );

    PLUTO_Core_t core = PLUTO_Malloc(sizeof(struct PLUTO_Core));
    core->config = config;
    core->logger = logger; 
    core->signal_queue = PLUTO_CORE_CreateSigQueue(n_nodes * 4);
    
    core->core_register = PLUTO_CreateCoreRegister(logger);
    if(!core->core_register)
    {
        PLUTO_LoggerWarning(logger, "Unable to create CoreRegister...");
        PLUTO_Free(&core);
        return NULL;
    }

    core->state = PLUTO_CreateCoreState(
        n_nodes,
        config,
        binary_dir,
        core->core_register,
        logger
    );
   
    PLUTO_CoreStateEvent_t event;
    PLUTO_CoreStateEventForProcess(&event);
    if(PLUTO_CoreStateDispatchEvent(&core->state, &event))
    {
        PLUTO_DestroyCoreState(&core->state);
        PLUTO_DestroyCore(&core);
    } 

    return core;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
void PLUTO_DestroyCore(PLUTO_Core_t *core)
{
    assert(NULL != *core);
    PLUTO_DestroyCoreConfig(&(*core)->config);
    PLUTO_CORE_DestroySigQueue(&(*core)->signal_queue);
    PLUTO_Free(*core);
    *core = NULL;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
bool PLUTO_CoreSignalReceived(PLUTO_Core_t core, int signum, pid_t pid)
{
    PLUTO_CORE_SigQueueElement_t element = {0};
    if(SIGINT == signum)
    {
        //PLUTO_LoggerInfo(core->logger, "SIGINT received.");
        if(PLUTO_CORE_SigQueueTop(core->signal_queue, &element) && (SIGINT == element.signum))
        {
            //PLUTO_LoggerInfo(core->logger, "SIGINT was already on Top of the Sig-Queue. Ignore it...");
            return true;
        }
    }

    if(SIGCHLD == signum)
    {
        //PLUTO_LoggerInfo(core->logger, "SIGCHLD received.");
        if(PLUTO_CORE_SigQueueTop(core->signal_queue, &element) && (SIGCHLD == element.signum) && (pid == element.pid))
        {
            //PLUTO_LoggerInfo(core->logger, "Ignore SIGCHLD, because the Signal was already received for the given PID.");
            return false;
        }
    }
    
    element.signum = signum;
    element.pid = pid;
    return PLUTO_CORE_SigQueuePut(core->signal_queue, element);
}
//
// --------------------------------------------------------------------------------------------------------------------
//
bool PLUTO_CoreProcess(PLUTO_Core_t core)
{
    PLUTO_CORE_SigQueueElement_t element = {0};
    while(PLUTO_CORE_SigQueueGet(core->signal_queue, &element))
    {
        PLUTO_LoggerInfo(core->logger, "Process Signal %i", element.signum);
        //
        // SIGINT -> Terminate all Subprocesses
        // SIGCHLD -> Restart Subprocess, but log the Crash!
        //
        PLUTO_CoreStateEvent_t event;
        PLUTO_CoreStateEventForSignal(&event, element.signum, element.pid);
        if(PLUTO_CoreStateDispatchEvent(&core->state, &event))
        {
            PLUTO_LoggerInfo(core->logger, "Terminate due to accepting State 0*...");
            return false;
        }
    }
    if(PLUTO_CoreStateAccepting(&core->state))
    {
        PLUTO_LoggerInfo(core->logger, "Terminate due to accepting State 1*...");
        return false;
    }
    return true;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
static const char* PLUTO_GetBinaryDirectory(PLUTO_Logger_t logger)
{
    char *binary_dir = getenv("PLUTO_BINARY_DIR");
    if(binary_dir)
    {
        PLUTO_LoggerInfo(logger, "Set PLUTO_BINARY_DIR to %s", binary_dir);
        return binary_dir;
    }
    static const char *default_binary_dir = "/usr/local/bin/";
    PLUTO_LoggerInfo(logger, "PLUTO_BINARY_DIR not set, fall back to Default %s", default_binary_dir);
    return default_binary_dir;
}
