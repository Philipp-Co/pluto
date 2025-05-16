#ifndef __PLUTO_CORE_H__
#define __PLUTO_CORE_H__

#include <pluto/pluto_core/data_layer/pluto_sig_queue.h>
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/pluto_core/application_layer/pluto_node_state.h>
#include <pluto/pluto_core/application_layer/pluto_core_state.h>

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

struct PLUTO_Core
{
    struct PLUTO_CoreState state;
    PLUTO_CORE_SigQueue_t signal_queue;
    PLUTO_Logger_t logger;
};
typedef struct PLUTO_Core* PLUTO_Core_t;


PLUTO_Core_t PLUTO_CreateCore(const char *filename, PLUTO_Logger_t logger);
void PLUTO_DestroyCore(PLUTO_Core_t *core);

bool PLUTO_CoreSignalReceived(PLUTO_Core_t core, int signum, pid_t pid);
bool PLUTO_CoreProcess(PLUTO_Core_t core);

#endif
