
//
// --------------------------------------------------------------------------------------------------------------------
//
#include "pluto/pluto_core/application_layer/pluto_node_state.h"
#include <pluto/pluto_core/application_layer/pluto_core_state.h>
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_malloc.h>
#include <pluto/pluto_core/data_layer/application_return_codes.h>

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

int32_t PLUTO_CoreStateFindByPid(struct PLUTO_CoreState *state, pid_t pid);
static bool PLUTO_CoreSetUpNodes(struct PLUTO_CoreState *core);
static void PLUTO_CoreStartNode(struct PLUTO_CoreState *core, int32_t index); 


PLUTO_CoreStateName_t PLUTO_CoreStateHandleSigInt(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event);
PLUTO_CoreStateName_t PLUTO_CoreStateHandleSigChld(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event);
PLUTO_CoreStateName_t PLUTO_CoreStateProcess(struct PLUTO_CoreState *state);


static PLUTO_CoreStateName_t PLUTO_CoreStateHandleInitial(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event);
static PLUTO_CoreStateName_t PLUTO_CoreStateHandleRunning(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event);
static PLUTO_CoreStateName_t PLUTO_CoreStateHandleTerminating(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event);
static PLUTO_CoreStateName_t PLUTO_CoreStateHandleTerminated(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event);

//
// --------------------------------------------------------------------------------------------------------------------
//

struct PLUTO_CoreState PLUTO_CreateCoreState(size_t n_nodes, PLUTO_CoreConfig_t config, const char *binary_directory, PLUTO_Logger_t logger)
{
    struct PLUTO_CoreState state = {
        .config = config,
        .current_state = PLUTO_CORE_STATE_NAME_INITIAL,
        .n_nodes = n_nodes,
        .nodes = PLUTO_Malloc(sizeof(struct PLUTO_NodeState) * n_nodes),
        .logger = logger,
        .binary_directory = PLUTO_Malloc(strlen(binary_directory) + 1)
    };
    memcpy(state.binary_directory, binary_directory, strlen(binary_directory) + 1);
   
    for(size_t i=0;i<n_nodes;++i)
    {
        state.nodes[i] = PLUTO_NodeState(
            config->configurations[i]
        );
    }

    PLUTO_LoggerInfo(state.logger, "Prepare %lu nodes...", state.n_nodes);
    return state;
}
//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_DestroyCoreState(struct PLUTO_CoreState *state)
{
    for(size_t i=0;i<state->n_nodes;++i)
    {
        PLUTO_DestroyNodeState(&state->nodes[i]);
    }
    PLUTO_Free((*state).nodes); 
    (*state).nodes = NULL;
    (*state).n_nodes = 0;
    (*state).current_state = PLUTO_CORE_STATE_NAME_TERMINATED;
}
//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_CoreStateEventForSignal(PLUTO_CoreStateEvent_t *event, int signum, pid_t pid)
{
    switch(signum)
    {
        case SIGINT:
            event->name = PLUTO_CORE_EVENT_NAME_SIGINT; 
            break;
        case SIGCHLD:
            event->name = PLUTO_CORE_EVENT_NAME_SIGCHLD; 
            break;
        default:
            event->name = PLUTO_CORE_EVENT_NAME_SIG_UNKNOWN; 
            break;
    }
    event->event.signal.signum = signum;
    event->event.signal.pid = pid;
}
//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_CoreStateEventForProcess(PLUTO_CoreStateEvent_t *event)
{
    event->name = PLUTO_CORE_EVENT_NAME_PROCESS;
    event->event.signal.signum = -1;
    event->event.signal.pid = 0;
}

//
// --------------------------------------------------------------------------------------------------------------------
//
bool PLUTO_CoreStateDispatchEvent(struct PLUTO_CoreState *state, PLUTO_CoreStateEvent_t *event)
{
    switch(state->current_state)
    {
        case PLUTO_CORE_STATE_NAME_INITIAL:
            state->current_state = PLUTO_CoreStateHandleInitial(state, event);
            break;
        case PLUTO_CORE_STATE_NAME_RUNNING:
            state->current_state = PLUTO_CoreStateHandleRunning(state, event);
            break;
        case PLUTO_CORE_STATE_NAME_TERMINATING:
            state->current_state = PLUTO_CoreStateHandleTerminating(state, event);
            break;
        case PLUTO_CORE_STATE_NAME_TERMINATED:
        default:
            PLUTO_LoggerInfo(state->logger, "Accepting...");
            state->current_state = PLUTO_CoreStateHandleTerminated(state, event);
            return true; 
    }
    return false;


}
//
// --------------------------------------------------------------------------------------------------------------------
//

static PLUTO_CoreStateName_t PLUTO_CoreStateHandleInitial(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event)
{
    switch(event->name)
    {
        case PLUTO_CORE_EVENT_NAME_PROCESS:
            PLUTO_LoggerInfo(state->logger, "Starting nodes...");
            if(!PLUTO_CoreSetUpNodes(state))
            {
                PLUTO_LoggerInfo(state->logger, "Transition to TERMINATED");
                return PLUTO_CORE_STATE_NAME_TERMINATED;
            }

            PLUTO_LoggerInfo(state->logger, "Transition to RUNNING");
            return PLUTO_CORE_STATE_NAME_RUNNING;
        default:
            PLUTO_LoggerInfo(state->logger, "Transition to TERMINATED");
            return PLUTO_CORE_STATE_NAME_TERMINATED;
    }
}

//
// --------------------------------------------------------------------------------------------------------------------
//
static PLUTO_CoreStateName_t PLUTO_CoreStateHandleRunning(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event)
{
    switch(event->name)
    {
        case PLUTO_CORE_EVENT_NAME_SIGINT:
            return PLUTO_CoreStateHandleSigInt(state, event);
        case PLUTO_CORE_EVENT_NAME_SIGCHLD:
            return PLUTO_CoreStateHandleSigChld(state, event);
        case PLUTO_CORE_EVENT_NAME_SIG_UNKNOWN:
            return PLUTO_CORE_STATE_NAME_RUNNING;
        case PLUTO_CORE_EVENT_NAME_PROCESS:
        default:
            return PLUTO_CORE_STATE_NAME_RUNNING;
    }
}

PLUTO_CoreStateName_t PLUTO_CoreStateHandleSigChld(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event)
{
    int32_t index = PLUTO_CoreStateFindByPid(state, event->event.signal.pid);
    if(index >= 0)
    {
        int return_value = 0;
        const pid_t result = waitpid(event->event.signal.pid, &return_value, WNOHANG);
        if(result < 0)
        {
            PLUTO_LoggerError(state->logger, "Error while waiting in Subprocess: %s", strerror(errno));
        }
        else if(result > 0)
        {
            if(WIFEXITED(return_value))
            {
                PLUTO_LoggerWarning(
                    state->logger,
                    "Subprocess exited with Status %i", 
                    (int8_t)WEXITSTATUS(return_value)
                );
                PLUTO_NodeStateTerminated(
                    &state->nodes[index],
                    (int)((int8_t)WEXITSTATUS(return_value))
                );
            }
            else if(WIFSIGNALED(return_value))
            {
                PLUTO_LoggerWarning(
                    state->logger,
                    "Subprocess was terminated by Signal: \"%s\"", 
                    strsignal(WTERMSIG(return_value))
                );
                PLUTO_NodeStateTerminatedBySignal(
                    &state->nodes[index],
                    WTERMSIG(return_value)
                );
                if(PLUTO_CORE_NS_INCONSPICIOUS == PLUTO_NodeStateCurrentState(&state->nodes[index]) || PLUTO_CORE_NS_SUSPICIOUS == PLUTO_NodeStateCurrentState(&state->nodes[index]))
                {
                    // restart Subprocess... 
                    PLUTO_CoreStartNode(state, index);
                }
            }
        }
    }

    for(size_t i=0;i<state->n_nodes;++i)
    {
        PLUTO_LoggerInfo(state->logger, "  Node %lu accepting => %i", i, (int)PLUTO_NodeStateAccepting(&state->nodes[i]));
        if(!PLUTO_NodeStateAccepting(&state->nodes[i]))
        {
            return PLUTO_CORE_STATE_NAME_RUNNING;
        }
    }
    PLUTO_LoggerInfo(state->logger, "Terminating, because no Subprocesses to manage are left...");
    return PLUTO_CORE_STATE_NAME_TERMINATED;
}

PLUTO_CoreStateName_t PLUTO_CoreStateHandleSigInt(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event)
{
    (void)event;
    //
    // Try to terminate all Subprocesses...
    //
    for(size_t i=0;i<state->n_nodes;++i)
    {
        const pid_t pid = PLUTO_NodeStateGetPid(&state->nodes[i]);
        (void)kill(pid, SIGINT);
        PLUTO_LoggerInfo(state->logger, "Terminate Subprocess %i with SIGINT", pid);
    }
    //
    // Each terminated Process will emit a SIGCHLD.
    // The processing is defered and done at a later point in time.
    //
    PLUTO_LoggerInfo(state->logger, "Handle SIGINT, return TERMINATING");
    return PLUTO_CORE_STATE_NAME_TERMINATING;
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static PLUTO_CoreStateName_t PLUTO_CoreStateHandleTerminating(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event)
{
    (void)state;
    //
    // Wait for all Subprocesses to terminate.
    //
    switch(event->name)
    {
        case PLUTO_CORE_EVENT_NAME_SIGCHLD:
            PLUTO_LoggerInfo(state->logger, "Wait %lu", event->event.signal.pid);
            int result;
            waitpid(event->event.signal.pid, &result, WNOHANG);
            int32_t index = PLUTO_CoreStateFindByPid(state, event->event.signal.pid);
            PLUTO_NodeStateTerminated(&state->nodes[index], 0);
            PLUTO_LoggerInfo(state->logger, "Handle Terminating, return TERMINATING");
            for(size_t i=0;i<state->n_nodes;++i)
            {
                if(!PLUTO_NodeStateAccepting(&state->nodes[index]))
                {
                    PLUTO_LoggerInfo(state->logger, "Unable to terminate, waiting for more Subprocesses");
                    return PLUTO_CORE_STATE_NAME_TERMINATING;
                }
            }
            PLUTO_LoggerInfo(state->logger, "No no Subprocesses... Terminated");
            return PLUTO_CORE_STATE_NAME_TERMINATED;
        default:
            PLUTO_LoggerInfo(state->logger, "Default Transition... TERMINATING");
            return PLUTO_CORE_STATE_NAME_TERMINATING;
    }
}
//
// --------------------------------------------------------------------------------------------------------------------
//

static PLUTO_CoreStateName_t PLUTO_CoreStateHandleTerminated(struct PLUTO_CoreState *state, const PLUTO_CoreStateEvent_t *event)
{
    (void)state;
    (void)event;
    return PLUTO_CORE_STATE_NAME_TERMINATED;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
void PLUTO_CoreStateSetPid(struct PLUTO_CoreState *state, size_t index, pid_t pid)
{
    assert(index < state->n_nodes);
    state->nodes[index].pid = pid;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
int32_t PLUTO_CoreStateFindByPid(struct PLUTO_CoreState *state, pid_t pid)
{
    for(int32_t i=0;i<(int32_t)state->n_nodes;++i)
    {
        if(pid == PLUTO_NodeStateGetPid(&state->nodes[i]))
        {
            return i;
        }
    }
    return -1;
}
//
// --------------------------------------------------------------------------------------------------------------------
//

void PLUTO_CoreCalculatePythonPath(char *buffer, int nbytes, const char *python_path)
{
    static const char *static_python_path = "/usr/lib/python38.zip;/usr/lib/python3.8;/usr/lib/python3.8/lib-dynload;/usr/local/lib/python3.8/dist-packages;/usr/lib/python3/dist-packages";
    snprintf(
        buffer,
        nbytes,
        "%s;%s",
        static_python_path,
        python_path
    );    
}

static void PLUTO_CoreStartNode(struct PLUTO_CoreState *core, int32_t index) 
{
    //
    // fork, exec
    //
    pid_t node_pid = fork();
    if(node_pid > 0)
    {
        // Parent Process
        PLUTO_NodeStateStarted(&core->nodes[index], node_pid);
    }
    else
    {
        char executable_path[4096];
        //
        // Child Process.
        // 
        // Go Exec!
        //
        if(PLUTO_CORE_CONFIG_NODE_TYPE_PYTHON == core->config->nodes[index].type)
        {
            if(!PLUTO_ConfigIsPythonPathSet(core->config->configurations[index]))
            {
                exit(PLUTO_CORE_RETURN_CODE_PYTHON_PATH_NOT_SET);
            }
            char python_path[4096];
            snprintf(executable_path, sizeof(executable_path), "%spluto_node_py", core->binary_directory);
            /*
            snprintf(
                python_path, 
                sizeof(python_path), 
                "%s", 
                PLUTO_ConfigPythonPath(core->config->configurations[index])
            );
            */
            PLUTO_CoreCalculatePythonPath(
                python_path, 
                sizeof(python_path), 
                PLUTO_ConfigPythonPath(core->config->configurations[index])
            );

            printf(
                "n: %s\nc: %s\ne: %s\np: %s\n",
                core->config->nodes[index].name,
                core->config->nodes[index].filename,
                core->config->nodes[index].executable,
                python_path
            );
            char * argv[] = 
            {
                executable_path,
                "-n",
                core->config->nodes[index].name,
                "-c",
                core->config->nodes[index].filename,
                "-e",
                core->config->nodes[index].executable,
                "-p",
                python_path,
                // "/usr/lib/python38.zip;/usr/lib/python3.8;/usr/lib/python3.8/lib-dynload;/usr/local/lib/python3.8/dist-packages;/usr/lib/python3/dist-packages;/pluto/",
                NULL
            };
            execv(executable_path, argv);
        }
        else if(PLUTO_CORE_CONFIG_NODE_TYPE_SHARED_LIB == core->config->nodes[index].type)
        {
            snprintf(executable_path, sizeof(executable_path), "%spluto_node_sh", core->binary_directory);
            char * argv[] = 
            {
                executable_path,
                "-n",
                core->config->nodes[index].name,
                "-c",
                core->config->nodes[index].filename,
                "-e",
                core->config->nodes[index].executable,
                NULL
            };
            execv(executable_path, argv);
        }
        else
        {
            snprintf(executable_path, sizeof(executable_path), "%spluto_node_pt", core->binary_directory);
            char * argv[] = 
            {
                executable_path,
                "-n",
                core->config->nodes[index].name,
                "-c",
                core->config->nodes[index].filename,
                NULL
            };
            execv(executable_path, argv);
        }
        // 
        // Exec only returns on Erro.
        // This will terminate the foreked Child Process on Error.
        //
        printf("Error: Execv returned with errno %s", strerror(errno));
        exit(-1);
    }
}

static bool PLUTO_CoreSetUpNodes(struct PLUTO_CoreState *core)
{
    //
    // This Process will try to create as many Child Processes as there are available
    // configurations for Nodes.
    //
    PLUTO_Config_t node_config = NULL;
    for(size_t i=0;i<core->config->n_configurations; ++i)
    {
        PLUTO_LoggerInfo(core->logger, "Start Node %i", i);
        node_config = PLUTO_CoreConfigGetNodeConfig(core->config, i);
        if(!node_config)
        {
            PLUTO_LoggerError(core->logger, "Error: Unable to read Nodeconfig %s", core->config->nodes[i].filename);
        }
        else
        {
            PLUTO_CoreStartNode(core, (uint32_t)i);
        }
    }
    return true;
}

bool PLUTO_CoreStateAccepting(const struct PLUTO_CoreState *state)
{
    return PLUTO_CORE_STATE_NAME_TERMINATED == state->current_state;
}
