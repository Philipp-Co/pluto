#ifndef __PLUTO_SYSTEM_EVENTS_H__
#define __PLUTO_SYSTEM_EVENTS_H__
//
// --------------------------------------------------------------------------------------------------------------------
//
#include <pluto/os_abstraction/config/pluto_compile_config.h>
//
// --------------------------------------------------------------------------------------------------------------------
//
#include <pluto/os_abstraction/pluto_time.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <stdint.h>
#include <sys/syslimits.h>
//
// --------------------------------------------------------------------------------------------------------------------
//
#define PLUTO_SE_OK 0
#define PLUTO_SE_NO_EVENT 1
#define PLUTO_SE_ERRROR -1
//
// --------------------------------------------------------------------------------------------------------------------
//
///
/// \brief  A System Event.
///
struct PLUTO_SystemEvent
{
    ///
    /// \brief  Timestamp of detection.
    ///
    PLUTO_Time_t timestamp;
    ///
    /// \brief  Filedescriptor.
    ///
    int descriptor;
} __attribute__((aligned(64)));
typedef struct PLUTO_SystemEvent* PLUTO_SystemEvent_t;

///
/// \brief  Create a new Instance of a System Event.
///         If not needed anymore, destroy with PLUTO_DestroySystemEvent().
///
PLUTO_SystemEvent_t PLUTO_CreateSystemEvent(void);
///
/// \brief  Destroy a Instance of a System Event.
///
void PLUTO_DestroySystemEvent(PLUTO_SystemEvent_t *event);
///
/// \brief  Get the Filedescriptor of the Entity which emitted this System Event.
///
int32_t PLUTO_SystemEventFiledescriptor(PLUTO_SystemEvent_t event);
//
// --------------------------------------------------------------------------------------------------------------------
//
///
///
///
struct PLUTO_SystemEventHandler;
typedef struct PLUTO_SystemEventHandler* PLUTO_SystemEventHandler_t;

///
/// \brief  Create an Instance of the System Event Handler.
///
PLUTO_SystemEventHandler_t PLUTO_CreateSystemEventHandler(PLUTO_Logger_t logger);
///
/// \brief  Destroy the System Event Handler.
///
void PLUTO_DestroySystemEventHandler(PLUTO_SystemEventHandler_t *handler);
///
/// \brief  Observe the given Filedescriptor and generate System Events for it.
///
int32_t PLUTO_SystemEventsHandlerRegisterObserver(PLUTO_SystemEventHandler_t handler, int descriptor);
int32_t PLUTO_SystemEventsHandlerDeregisterObserver(PLUTO_SystemEventHandler_t handler, int descriptor);
///
/// \brief  Check if any observed Filedescriptor has generated a System Event and return it.
/// \param[out] event   On Success the Functions will store Events in this Localtion.
/// \return int32_t Returns PLUTO_SE_OK if the function was able to return a System Event through the 
///                 "event" Argument.     
///                 PLUTO_SE_NO_EVENT if there was not Event to return, the "event" Argument was not changed.
///                 PLUTO_SE_ERROR if there was an Error, the "event" Argument was not changed.
///
int32_t PLUTO_SystemEventsPoll(PLUTO_SystemEventHandler_t handler, PLUTO_SystemEvent_t event); 
//
// --------------------------------------------------------------------------------------------------------------------
//
#endif
