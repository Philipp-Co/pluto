
#include "pluto/pluto_config/pluto_config.h"
#include <pluto/pluto_core/application_layer/pluto_node_state.h>
#include <pluto/pluto_core/data_layer/pluto_core_register.h>

struct PLUTO_NodeState PLUTO_NodeState(PLUTO_Config_t config, struct PLUTO_NodeStateData data)
{
    struct PLUTO_NodeState state = {
        .current_state = PLUTO_CORE_NS_INITIAL,
        .pid = 0,
        .exit_status = -1,
        .signum = {-1, -1, -1},
        .index = 0,
        .config = config,
        .data = data
    };
    return state;
}

void PLUTO_DestroyNodeState(struct PLUTO_NodeState *state)
{
    if((*state).config)
    {
        PLUTO_DestroyConfig(&(*state).config);
    }
}

PLUTO_ConstConfig_t PLUTO_NodeStateGetConfig(const PLUTO_NodeState_t state)
{
    return (PLUTO_ConstConfig_t)state->config;
}

void PLUTO_NodeStateStarted(PLUTO_NodeState_t state, pid_t pid)
{
    state->pid = pid;
    PLUTO_CoreRegisterSetNodePid(state->data.core_register, state->data.index, pid);
    switch(state->current_state)
    {
        case PLUTO_CORE_NS_INITIAL:
            state->current_state = PLUTO_CORE_NS_INCONSPICIOUS;
            break;
        default:
            state->current_state = PLUTO_CORE_NS_BROKEN;
            break;
    }
}

void PLUTO_NodeStateTerminated(PLUTO_NodeState_t state, int exitstatus)
{
    PLUTO_CoreRegisterSetNodePid(state->data.core_register, state->data.index, 0);
    switch(state->current_state)
    {
        case PLUTO_CORE_NS_INCONSPICIOUS:
        case PLUTO_CORE_NS_SUSPICIOUS:
            state->current_state = PLUTO_CORE_NS_TERMINATED;
            state->exit_status = exitstatus;
            break;
        default:
           break; 
    }
}

void PLUTO_NodeStateBroken(PLUTO_NodeState_t state)
{
    state->current_state = PLUTO_CORE_NS_BROKEN; 
}

void PLUTO_NodeStateReset(PLUTO_NodeState_t state)
{
    switch(state->current_state)
    {
        case PLUTO_CORE_NS_TERMINATED:
        case PLUTO_CORE_NS_BROKEN:
            state->current_state = PLUTO_CORE_NS_INITIAL;
            state->exit_status = 0;
            state->index = 0;
        default:
            break;
    }
}

void PLUTO_NodeStateTerminatedBySignal(PLUTO_NodeState_t state, int signum)
{
    switch(state->current_state)
    {
        case PLUTO_CORE_NS_INITIAL:
        case PLUTO_CORE_NS_INCONSPICIOUS:
                state->current_state = PLUTO_CORE_NS_SUSPICIOUS;
                state->signum[state->index++] = signum;
                break;
        case PLUTO_CORE_NS_SUSPICIOUS:
            if(state->index < 3)
            {
                state->current_state = PLUTO_CORE_NS_SUSPICIOUS;
                state->signum[state->index++] = signum;
            }
            else
            {
                state->current_state = PLUTO_CORE_NS_BROKEN;
            }
            break;
        default:
            state->current_state = PLUTO_CORE_NS_BROKEN;
            break;
    }
}

PLUTO_CORE_NodeStateValue_t PLUTO_NodeStateCurrentState(PLUTO_NodeState_t state)
{
    return state->current_state;
}


pid_t PLUTO_NodeStateGetPid(const PLUTO_NodeState_t state)
{
    return state->pid;
}

bool PLUTO_NodeStateAccepting(const PLUTO_NodeState_t state)
{
    return (PLUTO_CORE_NS_BROKEN == state->current_state) || (PLUTO_CORE_NS_TERMINATED == state->current_state);
}
