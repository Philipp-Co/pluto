#ifndef __PLUTO_SHARED_LIBRARY_H__
#define __PLUTO_SHARED_LIBRARY_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/application_layer/pluto_processor.h>

#include <stdbool.h>


//
// --------------------------------------------------------------------------------------------------------------------
//

bool PLUTO_SHLIB_Initialize(const char *path);
void PLUTO_SHLIB_Destroy(void);
PLUTO_ProcessorCallbackOutput_t PLUTO_SHLIB_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
