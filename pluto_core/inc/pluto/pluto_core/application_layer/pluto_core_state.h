//
// --------------------------------------------------------------------------------------------------------------------
//
///
/// \brief  This Module implements CoreState.
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
#include <pluto/pluto_core/data_layer/pluto_core_register.h>

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Names of internal States.
///
typedef enum
{
    PLUTO_CORE_STATE_NAME_INITIAL,
    PLUTO_CORE_STATE_NAME_RUNNING,
    PLUTO_CORE_STATE_NAME_TERMINATING,
    PLUTO_CORE_STATE_NAME_TERMINATED
} PLUTO_CoreStateName_t;

///
/// \brief  Structure of the CoreState.
///         initial:
///             The initial State.
///             Reloading Configuration is allowed.
///
///         running:
///             This State indicates that everything is properly set up and running.
///             Reloading Configuration is not allowed.
///
///         terminating:
///             A Termination is initiated. We are waiting for Termination of all managed Nodes.
///             Reloading Configuration is not allowed.
///
///         terminated:
///             All managed Nodes have terminated. Now we are ready to be shut down.
///             Reloading Configuration is not allowed.
///
///
///
///  -> initial <-> running ------+
///         |                     |
///         |                     V
///         +-> terminated <- terminating
///
///
struct PLUTO_CoreState
{
    PLUTO_CoreConfig_t config;
    char *config_path;
    char *binary_directory;
    PLUTO_CoreStateName_t current_state; 
    struct PLUTO_NodeState *nodes;
    size_t n_nodes;
    PLUTO_Logger_t logger;
    PLUTO_CoreRegister_t core_register;
};

///
/// \brief  List of Named Events.
///
typedef enum
{
    PLUTO_CORE_EVENT_NAME_SIG_UNKNOWN,
    PLUTO_CORE_EVENT_NAME_TIMER_TICK,
    PLUTO_CORE_EVENT_NAME_SIGINT,
    PLUTO_CORE_EVENT_NAME_SIGCHLD,
    PLUTO_CORE_EVENT_NAME_PROCESS,
    PLUTO_CORE_EVENT_NAME_START,
    PLUTO_CORE_EVENT_NAME_STOP
} PLUTO_CoreStateEventName_t;

///
/// \brief  Structure of an Event.
///
typedef struct
{
    const char* str_name;
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

///
/// \brief  Create a new Instance of a CoreState.
///         A CoreState manages a given set of Nodes.
///         When not needed anymore destroy this Instance with a Call to PLUTO_DestroyCoreState().
/// \return Returns a new Instance by Value. 
///
struct PLUTO_CoreState PLUTO_CreateCoreState(size_t n_nodes, const char *config_path, const char *binary_directory, PLUTO_CoreRegister_t core_register, PLUTO_Logger_t logger);
///
/// \brief  Destroys a given Instance created with a Call to PLUTO_CreateCoreState().
///
void PLUTO_DestroyCoreState(struct PLUTO_CoreState *state);
///
/// \brief  Initially start the given Nodes.
///         Call this Function before doing anything else!
///
void PLUTO_CoreStateStartNodes(struct PLUTO_CoreState *state);
///
/// \brief  Create an Event from a given Unix Signum and a PID.
/// \param[out] event - This Argument must not be NULL. The Result of the transformation is stored at the given Address.
///
void PLUTO_CoreStateEventForSignal(PLUTO_CoreStateEvent_t *event, int signum, pid_t pid);
///
/// \brief  Creates a specific Event.
///
void PLUTO_CoreStateEventForProcess(PLUTO_CoreStateEvent_t *event);
///
/// \brief  Dispatch the given Event to the given Instance.
///
bool PLUTO_CoreStateDispatchEvent(struct PLUTO_CoreState *state, PLUTO_CoreStateEvent_t *event);
///
/// \brief  Check if the given Instance is in an accepting State.
///         If in an accepting State, the internal Statemachine will not change anymore.
/// \return true if in an accepting State, false otherwise.
///
bool PLUTO_CoreStateAccepting(const struct PLUTO_CoreState *state);
//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
