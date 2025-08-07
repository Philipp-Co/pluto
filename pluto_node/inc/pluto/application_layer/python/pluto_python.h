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
typedef int (*PLUTO_PythonCAPI_RegisterFileObserver)(int filedescriptor);
typedef int (*PLUTO_PythonCAPI_DeregisterFileObserver)(int filedescriptor);
typedef int (*PLUTO_PythonCAPI_EmitEvent)(int id, int event, const char *payload, size_t nbytes);
typedef struct 
{
    PLUTO_PythonCAPI_RegisterFileObserver register_file_observer;
    PLUTO_PythonCAPI_DeregisterFileObserver deregister_file_observer;
    PLUTO_PythonCAPI_EmitEvent emit_event;
} PLUTO_PythonCAPI_t;
//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Initialize the Python Interpreter.
/// \param[in] python_path - A C-String. Contains a ";"-separated List of Directory-Paths.
/// \param[in] executable - A C-String. Contains the Path to the Main Pythonfile which is executad.
/// \param[in] logger - A Logger.
///
bool PLUTO_InitializePython(
    const char *python_path,
    const char *executable, 
    PLUTO_PythonCAPI_t *c_api, 
    PLUTO_Logger_t logger
);
void PLUTO_DeinitializePython(void);
PLUTO_ProcessorCallbackOutput_t PLUTO_PY_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
