#ifndef __PLUTO_CORE_NODE_STATE_H__
#define __PLUTO_CORE_NODE_STATE_H__

#include <time.h>
#include <unistd.h>


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
    PLUTO_CORE_NodeStateValue_t current_state;
    pid_t pid;
    int exit_status;
    int signum[3];
    time_t timestamps[3];
    int index;
};
typedef struct PLUTO_NodeState* PLUTO_NodeState_t;


struct PLUTO_NodeState PLUTO_NodeState(void);
void PLUTO_NodeStateStarted(PLUTO_NodeState_t state, pid_t pid);
void PLUTO_NodeStateTerminated(PLUTO_NodeState_t state, int exitstatus);
void PLUTO_NodeStateBroken(PLUTO_NodeState_t state);
void PLUTO_NodeStateReset(PLUTO_NodeState_t state);
void PLUTO_NodeStateTerminatedBySignal(PLUTO_NodeState_t state, int signum);
PLUTO_CORE_NodeStateValue_t PLUTO_NodeStateCurrentState(PLUTO_NodeState_t state);
pid_t PLUTO_NodeStateGetPid(const PLUTO_NodeState_t state);

#endif
