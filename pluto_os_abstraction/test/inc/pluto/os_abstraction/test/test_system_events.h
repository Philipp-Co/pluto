#ifndef __PLUTO_SYSTEM_EVENTS_H__
#define __PLUTO_SYSTEM_EVENTS_H__

///
/// \brief  Check if it is possible to create a SystemEventHandler.
///
void PLUTO_TEST_SystemEventsInitial(void); 
///
/// \brief  Check if it is possible to register an Observer for Filechanges.
///         A SystemEventHandler is instantiated and a File is created.
///         The SystemEventHandler is given the Filedescriptor for observation.
///         Later during the test the Files content will be changed.
///         Expect the SystemEventHandler to catch the Changeevent and return it to the User.
///
void PLUTO_TEST_SystemEventsFileEvent(void);
///
/// \brief  Check if it is possible to derigster a Fileevent Observer.
///         Create a File, register it to be observed and deregister it later.
///         Change the Files contents.
///         Expect that no Events are generated.
///
void PLUTO_TEST_SystemEventsDeregisterFileObserver(void);

#endif
