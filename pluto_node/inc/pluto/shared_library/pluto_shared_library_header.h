#ifndef __PLUTO_SHARED_LIBRARY_CALLBACK_H__
#define __PLUTO_SHARED_LIBRARY_CALLBACK_H__

#include <pluto/types/pluto_types.h>


void PLUTO_SharedLibrarySetupObject(void);
void PLUTO_SharedLibraryTearDownObject(void);
PLUTO_ProcessorCallbackOutput_t PLUTO_SharedLibraryProcessEvent(PLUTO_ProcessorCallbackInput_t *args);

#endif

