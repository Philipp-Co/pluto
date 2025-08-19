//
// --------------------------------------------------------------------------------------------------------------------
//
///
/// \brief  This Module handles the Node State. 
///
//
// --------------------------------------------------------------------------------------------------------------------
//
#ifndef __PLUTO_CORE_NODE_STATE_H__
#define __PLUTO_CORE_NODE_STATE_H__

#include "pluto/pluto_config/pluto_config.h"
#include <pluto/pluto_core/data_layer/pluto_core_register.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

struct PLUTO_NodeStateData
{
    PLUTO_CoreRegister_t core_register;
    int32_t index;
};

typedef struct 
{
    char *python_path;
} PLUTO_NodeStatePythonConfig_t;
///
/// \brief  State-Enum.
///
typedef enum
{
    PLUTO_CORE_NS_INITIAL,
    PLUTO_CORE_NS_INCONSPICIOUS,
    PLUTO_CORE_NS_TERMINATED,
    PLUTO_CORE_NS_SUSPICIOUS,
    PLUTO_CORE_NS_BROKEN
} PLUTO_CORE_NodeStateValue_t;
///
/// 
/// initial: 
///     Process is not yet started.
///
/// inconspicious:
///     Process is running wihtout any crashes.
///
/// suspicious:
///     Process has crashed, but was restarted.
///     A Process which is running suspicious can evolve to a Process which is running ok.
///     For this there must not be any crashes for at least 1 Minute.
///     The Core trys to restart suspicous Processes up to 3 Times.
///
/// terminated:
///     The Process has exited its Main.
///     The Core does not restart the Process.
///
/// broken:
///     The Process was either terminated by its Core or is not started again.
///
///
///         +-----------------------------------------------+
///         |           +-----------+                       |
///         |           |           |                       |
///         |           V           |                       V
///     initial -> running_ok -> running_suspicous -> terminated_broken
///                     |           |
///                     V           |
///                 terminated <----+
///
struct PLUTO_NodeState
{
    PLUTO_Config_t config;
    struct PLUTO_NodeStateData data;
    int signum[3];
    time_t timestamps[3];
    PLUTO_CORE_NodeStateValue_t current_state;
    pid_t pid;
    int exit_status;
    int index;
};
typedef struct PLUTO_NodeState* PLUTO_NodeState_t;
//
// --------------------------------------------------------------------------------------------------------------------
//
///
/// \brief  Create a State-Object.
///
struct PLUTO_NodeState PLUTO_NodeState(PLUTO_Config_t config, struct PLUTO_NodeStateData data);
///
/// \brief  Destroy a State-Object.
/// \pre    The "state" must not be NULL.
///
void PLUTO_DestroyNodeState(struct PLUTO_NodeState *state);
PLUTO_ConstConfig_t PLUTO_NodeStateGetConfig(const PLUTO_NodeState_t state);
///
/// \brief  Start Event.
///
void PLUTO_NodeStateStarted(PLUTO_NodeState_t state, pid_t pid);
///
/// \brief  Terminated Event.
///
void PLUTO_NodeStateTerminated(PLUTO_NodeState_t state, int exitstatus);
///
/// \brief  Broken Event.
///
void PLUTO_NodeStateBroken(PLUTO_NodeState_t state);
///
/// \brief  Reset the State-Object.
///
void PLUTO_NodeStateReset(PLUTO_NodeState_t state);
///
/// \brief  Signal Received Event.
///
void PLUTO_NodeStateTerminatedBySignal(PLUTO_NodeState_t state, int signum);
///
/// \brief  Get the current State of the given State-Object.
///
PLUTO_CORE_NodeStateValue_t PLUTO_NodeStateCurrentState(PLUTO_NodeState_t state);
///
/// \brief  Get the assigned PID of the given Object.
/// 
pid_t PLUTO_NodeStateGetPid(const PLUTO_NodeState_t state);
///
/// \brief  Check if the given Object is in an accepting State.
///         In an accepting State no Eventinput triggers another State change.
/// \return True if accepting, false otherwise.
///
bool PLUTO_NodeStateAccepting(const PLUTO_NodeState_t state);
//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
