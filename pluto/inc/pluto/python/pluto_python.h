#ifndef __PLUTO_PYTHON_H__
#define __PLUTO_PYTHON_H__

#include <stdbool.h>
#include <pluto/pluto_processor.h>


bool PLUTO_InitializePython(void);
void PLUTO_DeinitializePython(void);
PLUTO_ProcessorCallbackOutput_t PLUTO_PY_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args);

#endif
