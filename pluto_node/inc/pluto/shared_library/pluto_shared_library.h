#ifndef __PLUTO_SHARED_LIBRARY_H__
#define __PLUTO_SHARED_LIBRARY_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <stdbool.h>
#include <pluto/pluto_processor.h>


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
