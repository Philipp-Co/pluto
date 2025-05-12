#include "pluto/pluto_core/application_layer/pluto_node_state.h"
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


static bool PLUTO_CoreSetUpNodes(PLUTO_Core_t core);
static int32_t PLUTO_CoreFindNodeByPid(PLUTO_Core_t core, pid_t pid);

static void PLUTO_CoreStartNode(PLUTO_Core_t core, uint32_t index); 

PLUTO_Core_t PLUTO_CreateCore(const char *filename, PLUTO_Logger_t logger)
{
    char *binary_dir = getenv("PLUTO_BINARY_DIR");
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

    PLUTO_Core_t core = PLUTO_Malloc(sizeof(struct PLUTO_Core));
    core->logger = logger; 
    core->signal_queue = PLUTO_CORE_CreateSigQueue(n_nodes * 4);
    core->nodes = PLUTO_Malloc(n_nodes * sizeof(struct PLUTO_NodeState));
    core->n_nodes = n_nodes;
    for(size_t i=0;i<n_nodes;++i)
    {
        core->nodes[i] = PLUTO_NodeState();
    }
    core->binary_directory = PLUTO_Malloc(strlen(binary_dir) + 1);
    memcpy(core->binary_directory, binary_dir, strlen(binary_dir) + 1);
    core->config = config;

    if(!PLUTO_CoreSetUpNodes(core))
    {
        PLUTO_DestroyCoreConfig(&config);
        PLUTO_DestroyCore(&core);
    }
    
    return core;
}

void PLUTO_DestroyCore(PLUTO_Core_t *core)
{
    assert(NULL != *core);

    PLUTO_DestroyCoreConfig(&(*core)->config);
    PLUTO_CORE_DestroySigQueue(&(*core)->signal_queue);
    PLUTO_Free((*core)->binary_directory);
    PLUTO_Free(*core);
    *core = NULL;
}

static bool PLUTO_CORE_CheckPid(const PLUTO_Core_t core, pid_t pid);

bool PLUTO_CoreSignalReceived(PLUTO_Core_t core, int signum, pid_t pid)
{
    PLUTO_CORE_SigQueueElement_t element = {0};
    if(SIGINT == signum)
    {
        if(PLUTO_CORE_SigQueueTop(core->signal_queue, &element) && (SIGINT == element.signum))
        {
            return true;
        }
    }

    if(SIGCHLD == signum)
    {
        if(!PLUTO_CORE_CheckPid(core, pid))
        {
            return false;
        }
        if(PLUTO_CORE_SigQueueTop(core->signal_queue, &element) && (SIGCHLD == element.signum) && (pid == element.pid))
        {
            return false;
        }
    }
    
    element.signum = signum;
    element.pid = pid;
    return PLUTO_CORE_SigQueuePut(core->signal_queue, element);
}

static bool PLUTO_CORE_CheckPid(const PLUTO_Core_t core, pid_t pid)
{
    for(size_t i=0;i<core->n_nodes;++i)
    {
        if(pid == PLUTO_NodeStateGetPid(&core->nodes[i]))
            return true;
    }
    return false;
}

bool PLUTO_CoreProcess(PLUTO_Core_t core)
{
    PLUTO_CORE_SigQueueElement_t element = {0};
    if(PLUTO_CORE_SigQueueGet(core->signal_queue, &element))
    {
        //
        // SIGINT -> Terminate all Subprocesses
        // SIGCHLD -> Restart Subprocess, but log the Crash!
        //
        if(SIGCHLD == element.signum)
        {
            int return_value = 0;
            const pid_t result = waitpid(element.pid, &return_value, WNOHANG);
            if(result < 0)
            {
                PLUTO_LoggerError(core->logger, "Error while waiting in Subprocess: %s\n", strerror(errno));
            }
            else if(result > 0)
            {
                if(WIFEXITED(return_value))
                {
                    PLUTO_LoggerWarning(
                        core->logger,
                        "Subprocess exited with Status %i", 
                        (int8_t)WEXITSTATUS(return_value)
                    );
                    PLUTO_NodeStateTerminated(
                        &core->nodes[PLUTO_CoreFindNodeByPid(core, element.pid)], 
                        (int)((int8_t)WEXITSTATUS(return_value))
                    );
                }
                else if(WIFSIGNALED(return_value))
                {
                    int32_t index = PLUTO_CoreFindNodeByPid(core, element.pid);
                    PLUTO_LoggerWarning(
                        core->logger,
                        "Subprocess was terminated by Signal: \"%s\"", 
                        strsignal(WTERMSIG(return_value))
                    );
                    PLUTO_NodeStateTerminatedBySignal(
                        &core->nodes[index], 
                        WTERMSIG(return_value)
                    );
                    if(PLUTO_CORE_NS_INCONSPICIOUS == PLUTO_NodeStateCurrentState(&core->nodes[index]) || PLUTO_CORE_NS_SUSPICIOUS == PLUTO_NodeStateCurrentState(&core->nodes[index]))
                    {
                        // restart Subprocess... 
                        PLUTO_CoreStartNode(core, index);
                    }
                }
            }
        }
        else if(SIGINT == element.signum)
        {
            //
            // Try to terminate all Subprocesses...
            //
            for(size_t i=0;i<core->n_nodes;++i)
            {
                const pid_t pid = PLUTO_NodeStateGetPid(&core->nodes[i]);
                (void)kill(pid, SIGINT);
            }
            //
            // Each terminated Process will emit a SIGCHLD.
            // The processing is defered and done at a later point in time.
            //
        }
    }
    else
    {
        // nothing to do...
    }
    return true;
}

static bool PLUTO_CoreSetUpNodes(PLUTO_Core_t core)
{
    //
    // This Process will try to create as many Child Processes as there are available
    // configurations for Nodes.
    //
    PLUTO_Config_t node_config = NULL;
    for(size_t i=0;i<core->config->n_configurations; ++i)
    {
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

static int32_t PLUTO_CoreFindNodeByPid(PLUTO_Core_t core, pid_t pid)
{
    for(int32_t i=0;i<(int32_t)core->n_nodes;++i)
    {
        if(pid == PLUTO_NodeStateGetPid(&core->nodes[i]))
            return i;
    }
    return -1;
}
                        
static void PLUTO_CoreStartNode(PLUTO_Core_t core, uint32_t index) 
{
    //
    // fork, exec
    //
    PLUTO_LoggerInfo(core->logger, "Start Node with Name %s, Configuration %s", core->config->nodes[index].name, core->config->nodes[index].filename);
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
            snprintf(executable_path, sizeof(executable_path), "%spluto_node_py", core->binary_directory);
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
                "/usr/lib/python38.zip;/usr/lib/python3.8;/usr/lib/python3.8/lib-dynload;/usr/local/lib/python3.8/dist-packages;/usr/lib/python3/dist-packages;/pluto/",
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
        printf("Error: Execv returned with errno %s\n", strerror(errno));
        exit(-1);
    }
}
