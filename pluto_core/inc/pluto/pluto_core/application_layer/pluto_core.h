//
// --------------------------------------------------------------------------------------------------------------------
//
///
/// \brief  This Module implements the Core functionality.
///         The Core handles the creation of Subprocesses which run Nodes.
///         Handling includes:
///             - creating/starting Nodes
///             - Handle Node crashes / restarting Nodes
///             - Handle shutdown
///
//
// --------------------------------------------------------------------------------------------------------------------
//
#ifndef __PLUTO_CORE_H__
#define __PLUTO_CORE_H__
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_core/data_layer/pluto_sig_queue.h>
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/pluto_core/application_layer/pluto_node_state.h>
#include <pluto/pluto_core/application_layer/pluto_core_state.h>
#include <pluto/pluto_core/data_layer/pluto_core_register.h>

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

//
// --------------------------------------------------------------------------------------------------------------------
//
///
/// \brief A Core Object. 
///
struct PLUTO_Core
{
    PLUTO_CoreConfig_t config;
    ///
    /// \brief  An Object which stores Data which has State.
    ///
    struct PLUTO_CoreState state;
    ///
    /// \brief  A Queue which stores received Signals.
    ///
    PLUTO_CORE_SigQueue_t signal_queue;
    ///
    /// \brief  A logger.
    ///
    PLUTO_Logger_t logger;
    PLUTO_CoreRegister_t core_register;
};
typedef struct PLUTO_Core* PLUTO_Core_t;
//
// --------------------------------------------------------------------------------------------------------------------
//
///
/// \brief  Create a new Core Object. 
///         Must be destroyed with a call to PLUTO_DestroyCore() after use.
/// \returns    A Core Object on Success. 
///             Returns NULL if the Object could not be created.
///
PLUTO_Core_t PLUTO_CreateCore(const char *filename, PLUTO_Logger_t logger);
///
/// \brief  Destroys a Core Object.
/// \pre    "core" must not be NULL. 
///
void PLUTO_DestroyCore(PLUTO_Core_t *core);
///
/// \brief  Inform the Core Object of a received Signal.
/// \returns    True if the Core Object has reached an accepting State,
///             which means the Object is ready to be destroyed.
///             False if there is more work to do. 
///
bool PLUTO_CoreSignalReceived(PLUTO_Core_t core, int signum, pid_t pid);
///
/// \brief  Process Events from the internal Eventqueue.
///         Should be called periodically.
///
bool PLUTO_CoreProcess(PLUTO_Core_t core);
//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
