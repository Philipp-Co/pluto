#ifndef __PLUTO_PYTHON_H__
#define __PLUTO_PYTHON_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/application_layer/pluto_processor.h>
#include <pluto/os_abstraction/pluto_logger.h>

#include <stdbool.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

bool PLUTO_InitializePython(const char *python_path, const char *executable, PLUTO_Logger_t logger);
void PLUTO_DeinitializePython(void);
PLUTO_ProcessorCallbackOutput_t PLUTO_PY_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
