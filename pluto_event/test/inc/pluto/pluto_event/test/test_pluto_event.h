#ifndef __PLUTO_EVENT_TEST__H__
#define __PLUTO_EVENT_TEST__H__

///
/// \brief  Initial Test.
///         Instantiate and Destroy an Event.
///
void PLUTO_TEST_TestEvent(void);
///
/// \brief  Create a Event from a given Buffer.
/// \assume The given Buffer contains a successfully parsable Object.
/// \expect The created Event contains all Information from the Buffer.
///
void PLUTO_TEST_EventFromBufferSuccess(void);
///
/// \brief  Try to create a Buffer from a Buffer with unusable Containt.
/// \assume The Buffer is unsable because of
///             - Key/Value Pair missing
///             - Key "id" missing
///             - Value for "id" missing
///             - Value < 0
/// \expect The Function will return NULL.
///
void PLUTO_TEST_EventFromBufferIdMissing(void);
///
/// \brief  Try to create a Buffer from a Buffer with unusable Containt.
/// \assume The Buffer is unsable because of
///             - Key/Value Pair missing
///             - Key "event" missing
///             - Value for "event" missing
///             - Value < 0
/// \expect The Function will return NULL.
///
void PLUTO_TEST_EventFromBufferEventMissing(void);
///
/// \brief  Try to create a Buffer from a Buffer with unusable Containt.
/// \assume The Buffer is unsable because of
///             - Key/Value Pair missing
///             - Key "payload" missing
///             - Value for "payload" missing
///             - Value < 0
/// \expect The Function will return NULL.
///
void PLUTO_TEST_EventFromBufferPayloadMissing(void);
///
/// \brief  Dump a given Event to a Buffer.
/// \assume -
/// \expect The given Event can be converted to a String.
///
void PLUTO_TEST_EventToString(void);


#endif
