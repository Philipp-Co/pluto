///
/// \brief  This Modules defines what an Event is.
///         An Event is a Dataobject. It can transport Information like
///         a Id, Event Id, Timestamp and a Payload.
///         Id is meant to be something like a correlation Id.
///         Event Id is meant to specify a Type for this Event.
///         The Idea for the Timestamp is to determine at which Point in Time an Event was created or Received.
///         The Event can store binary Data as Payload.
///
#ifndef __PLUTO_EVENT_H__
#define __PLUTO_EVENT_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/pluto_time.h>
#include <pluto/os_abstraction/pluto_message_queue.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

struct PLUTO_Event 
{
    ///
    /// \brief  User provided Id.
    ///
    uint32_t id;
    ///
    /// \breif  User provided Event Id.
    ///
    uint32_t eventid;
    ///
    /// \brief  A Timestamp.
    ///
    PLUTO_Time_t timestamp;
    ///
    /// \brief  Number of usable Bytes in "payload".
    ///
    size_t nbytes_payload;
    ///
    /// \brief  A Buffer.
    /// 
    struct PLUTO_MsgBuf payload;
};
typedef struct PLUTO_Event* PLUTO_Event_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Create a Event with a specific Buffersize.
/// \returns NULL on Error. On Success this Function returns a new Instance of a Event.
///
PLUTO_Event_t PLUTO_CreateEvent(void);
///
/// \brief  Destroy a given Event.
/// \param[in/out] event - Destroys the given Event *event and sets the Pointer to NULL.
///
void PLUTO_DestroyEvent(PLUTO_Event_t *event);
///
/// \brief  Create a new Event Instance from a given Buffer.
/// \param[in] buffer - A Buffer.
/// \param[in] nbytes - Size of the given Buffer in Bytes.
///
bool PLUTO_CreateEventFromBuffer(PLUTO_Event_t event, const char *buffer, size_t nbytes);
///
/// \brief  Dump the given Event Instance to the given Buffer.
/// \param[in] event - The Event that shall be dumped. Must not be NULL.
/// \param[out] buffer - The Destinationbuffer where the Event will be dumped to.
/// \param[in] nbytes - Size of the Buffer in Bytes.
/// \returns    false on Error.
///             true on Success, which means the given buffer was large enough to hold 
///             all data stored in the given Event.
///
bool PLUTO_EventToBuffer(const PLUTO_Event_t event, char *buffer, size_t nbytes);
///
/// \brief  Set the Id of the given Event.
/// \param[in] event - The Event. Must not be NULL.
/// \param[in] id - User defined Id.
///
void PLUTO_EventSetId(PLUTO_Event_t event, uint32_t id);
///
/// \brief  Set the User defined Event Id.
/// \param[in] event - The Event. Must not be NULL.
/// \param[in] eventid - The Event Id.
///
void PLUTO_EventSetEvent(PLUTO_Event_t event, uint32_t eventid);
///
/// \brief  Getter for the User defined Event Id.
/// \returns uint32_t - Event Id.
///
uint32_t PLUTO_EventEventId(const PLUTO_Event_t event);
///
/// \brief  Getter for the User defined Id.
/// \returns uint32_t - Id.
///
uint32_t PLUTO_EventId(const PLUTO_Event_t event);
///
/// \brief  Getter for the Events Timestamp.
/// \returns PLUTO_Time_t - Timestamp.
///
PLUTO_Time_t PLUTO_EventTimestamp(const PLUTO_Event_t event);
///
/// \brief  Set a Timestamp for the given Event.
/// \param[out] event - The Event Instance. Must not be NULL.
/// \param[in] timestamp - The Timestamp to set.
///
void PLUTO_EventSetTimestamp(PLUTO_Event_t event, PLUTO_Time_t timestamp);
///
/// \brief Getter for the preallocated Buffer for a possible Payload.
/// \param[in] event - The Event. Must not be NULL.
/// \returns char* - A Buffer. Use PLUTO_EventSizeOfPayloadBuffer(event) to retrieve the Size of this Buffer in Bytes.
///
char* PLUTO_EventPayload(PLUTO_Event_t event);
bool PLUTO_EventCopyBufferToPayload(PLUTO_Event_t event, const void *buffer, size_t nbytes);
///
/// \brief  Set the number of Bytes of the payload.
/// \param[out] event - The Event.
/// \param[in] nbytes_payload - Number of Bytes in the Payload Buffer. 
///
void PLUTO_EventSetSizeOfPayload(PLUTO_Event_t event, size_t nbytes_payload);
///
/// \brief  Get the number of usable Bytes in the Payload.
/// \param[in] event - The Event. Must not be NULL.
/// \param[out] size_t - Number of usable Bytes. 
///
size_t PLUTO_EventSizeOfPayload(const PLUTO_Event_t event);
///
/// \brief  Sie of the Payload Buffer.
/// \param[in] event - The Event.
/// \returns Size of the Payload Buffer in Bytes.
///
size_t PLUTO_EventSizeOfPayloadBuffer(const PLUTO_Event_t event);

//
// --------------------------------------------------------------------------------------------------------------------
//
#endif
