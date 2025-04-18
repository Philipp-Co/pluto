//
// --------------------------------------------------------------------------------------------------------------------
//
///
/// \brief  This Module implements Core State. 
///
//
// --------------------------------------------------------------------------------------------------------------------
//
#ifndef __PLUTO_CORE_STATE_H__
#define __PLUTO_CORE_STATE_H__
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/pluto_core/application_layer/pluto_node_state.h>

#include <stdbool.h>
#include <unistd.h>

//
// --------------------------------------------------------------------------------------------------------------------
//
typedef enum
{
    PLUTO_CORE_STATE_NAME_INITIAL,
    PLUTO_CORE_STATE_NAME_RUNNING,
    PLUTO_CORE_STATE_NAME_TERMINATING,
    PLUTO_CORE_STATE_NAME_TERMINATED
} PLUTO_CoreStateName_t;

struct PLUTO_CoreState
{
    PLUTO_CoreConfig_t config;
    char *binary_directory;
    PLUTO_CoreStateName_t current_state; 
    struct PLUTO_NodeState *nodes;
    size_t n_nodes;
    PLUTO_Logger_t logger;
};

typedef enum
{
    PLUTO_CORE_EVENT_NAME_SIG_UNKNOWN,
    PLUTO_CORE_EVENT_NAME_SIGINT,
    PLUTO_CORE_EVENT_NAME_SIGCHLD,
    PLUTO_CORE_EVENT_NAME_PROCESS
} PLUTO_CoreStateEventName_t;

typedef struct
{
    PLUTO_CoreStateEventName_t name;
    union
    {
        struct 
        {
            pid_t pid;
            int signum;
        } signal;
    } event;
} PLUTO_CoreStateEvent_t;
//
// --------------------------------------------------------------------------------------------------------------------
//

struct PLUTO_CoreState PLUTO_CreateCoreState(size_t n_nodes, PLUTO_CoreConfig_t config, const char *binary_directory, PLUTO_Logger_t logger);
void PLUTO_DestroyCoreState(struct PLUTO_CoreState *state);
void PLUTO_CoreStateStartNodes(struct PLUTO_CoreState *state);
void PLUTO_CoreStateEventForSignal(PLUTO_CoreStateEvent_t *event, int signum, pid_t pid);
void PLUTO_CoreStateEventForProcess(PLUTO_CoreStateEvent_t *event);
bool PLUTO_CoreStateDispatchEvent(struct PLUTO_CoreState *state, PLUTO_CoreStateEvent_t *event);
bool PLUTO_CoreStateAccepting(const struct PLUTO_CoreState *state);
//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
