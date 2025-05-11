#ifndef __PLUTO_CORE_H__
#define __PLUTO_CORE_H__

#include <pluto/pluto_core/data_layer/pluto_sig_queue.h>
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/pluto_core/application_layer/pluto_node_state.h>

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>


struct PLUTO_Core
{
    PLUTO_CoreConfig_t config;
    char *binary_directory;
    struct PLUTO_NodeState *nodes; 
    size_t n_nodes;
    PLUTO_CORE_SigQueue_t signal_queue;
};
typedef struct PLUTO_Core* PLUTO_Core_t;


PLUTO_Core_t PLUTO_CreateCore(const char *filename);
void PLUTO_DestroyCore(PLUTO_Core_t *core);

bool PLUTO_CoreSignalReceived(PLUTO_Core_t core, int signum, pid_t pid);
bool PLUTO_CoreProcess(PLUTO_Core_t core);

#endif
