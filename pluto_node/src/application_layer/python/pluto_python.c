//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/application_layer/pluto_compile_time_switches.h>
#include "pluto/application_layer/pluto_processor.h"
#include "pluto/os_abstraction/pluto_logger.h"
#include "pluto/os_abstraction/pluto_malloc.h"
#include "pluto/os_abstraction/pluto_time.h"
#include <pluto/application_layer/python/pluto_python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Python.h>
#include <wchar.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

//
//  class PlutoEventHandler:
//      def setup(self, argc, argv):
//          pass
//      def teardown(self):
//          pass
//      def run(self, id: int, event:int, payload: str) -> Optional[Tuple[int, int, str]]:
//          """Is executed for each Event.
//          
//          Args:
//              id: int: Events Id.
//              event: int: Events Type.
//              payload: str: A String.
//
//          Returns:
//              Optional[Tuple[int, int, str]]: 
//                  Returns a Tuple of Event Id, Event Type and Payload on success.
//                  Returns None on Error.
//          """
//          pass
//      pass
//

#define PLUTO_PYTHON_INTERFACE_CLASS "PlutoEventHandler"
#define PLUTO_PYTHON_SETUP_METHOD "setup"
#define PLUTO_PYTHON_TEARDOWN_METHOD "teardown"
#define PLUTO_PYTHON_RUN_METHOD "run"

//
// --------------------------------------------------------------------------------------------------------------------
//

typedef struct
{
    char **paths;
    size_t n_paths;
} PLUTO_PY_PythonPath_t __attribute__((aligned(64)));

//
// --------------------------------------------------------------------------------------------------------------------
//

static bool PLUTO_PY_ReadPythonPathsFromEnv(const char *python_path, PLUTO_PY_PythonPath_t *paths);
//static PyObject* PyInit_emb_input(void);
static PyObject* PLUTO_PY_CreateInterface(const char *path, PLUTO_Logger_t logger);

//
// --------------------------------------------------------------------------------------------------------------------
//

static PLUTO_ProcessorCallbackInput_t *PLUTO_PY_current_buffer;
static PLUTO_ProcessorCallbackOutput_t PLUTO_PY_current_output_buffer;
static PyObject *PLUTO_PY_interface_object = NULL;
PyGILState_STATE gstate;
PLUTO_Logger_t PLUTO_PY_logger = NULL;

static PyObject* PLUTO_PythonInitPluto(void);


static int PLUTO_PythonCAPI_DefaultRegisterObserver(int filedescriptor);
static int PLUTO_PythonCAPI_DefaultDeregisterObserver(int filedescriptor);
static int PLUTO_PythonCAPI_DefaultEmitEvent(int id, int event, const char *payload, size_t nbytes);

//
// --------------------------------------------------------------------------------------------------------------------
//

static PLUTO_PythonCAPI_t PLUTO_c_api = {
    .register_file_observer=PLUTO_PythonCAPI_DefaultRegisterObserver,
    .deregister_file_observer=PLUTO_PythonCAPI_DefaultDeregisterObserver,
    .emit_event=PLUTO_PythonCAPI_DefaultEmitEvent
};

//
// --------------------------------------------------------------------------------------------------------------------
//
bool PLUTO_InitializePython(
    const char *python_path,
    const char *executable,
    PLUTO_PythonCAPI_t *c_api, 
    PLUTO_Logger_t logger
)
{
    //
    // https://docs.python.org/3/c-api/init_config.html
    //
    PyStatus status;
    PyConfig config;
    if(NULL != c_api)
    { 
        assert(c_api->register_file_observer != NULL); 
        assert(c_api->deregister_file_observer != NULL); 
        assert(c_api->emit_event != NULL); 
        PLUTO_c_api = *c_api;
    }
    PLUTO_PY_logger = logger;

    PLUTO_LoggerInfo(logger, "Start Python configuration...");
    //PyImport_AppendInittab("pluto", &PyInit_emb_input);
    PyImport_AppendInittab("pluto", &PLUTO_PythonInitPluto);
    PyConfig_InitIsolatedConfig(&config);
    config.isolated = 1;
    
    // TODO: Check if venvs are needed...
    //char *env = getenv("VIRTUAL_ENV");
    //
    PLUTO_LoggerInfo(logger, "Compile Python Path Variable...");
    char *buffer = PLUTO_Malloc(4096);
    if(!buffer) return false;
    // TODO: Memory Management...
    const int N_PYTHON_PATHS = 128;
    PLUTO_PY_PythonPath_t python_path_buffer = {
        .paths=PLUTO_Malloc(sizeof(char*) * N_PYTHON_PATHS),
        .n_paths=0
    };
    for(size_t i=0LU;i<N_PYTHON_PATHS;++i)
    {
        python_path_buffer.paths[i] = PLUTO_Malloc(256);
        memset(python_path_buffer.paths[i], '\0', 256);
    }

    PLUTO_PY_ReadPythonPathsFromEnv(python_path, &python_path_buffer);
    for(size_t i=0;i<python_path_buffer.n_paths;++i)
        PLUTO_LoggerInfo(logger, "    %s", python_path_buffer.paths[i]);

    for(size_t j=0;j<python_path_buffer.n_paths;++j)
    {
        snprintf(buffer, 4096, "%s", python_path_buffer.paths[j]); 
        wchar_t *wcstr = PLUTO_Malloc(sizeof(wchar_t) * (strlen(buffer) + 1));
        for(size_t i=0;i<(strlen(buffer)+1);++i)
        {
            wcstr[i] = L'\0';
        } 
        mbstowcs(wcstr, buffer, strlen(buffer));
        PyWideStringList_Append(&config.module_search_paths, wcstr);
        PLUTO_Free(wcstr);
    }
    config.module_search_paths_set = 1;
    //
    PLUTO_LoggerInfo(logger, "Initialize Python Interpreter from Config.");
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status))
    {
        goto exception;
    }
    
    PLUTO_LoggerInfo(logger, "Read Interface from Script: %s", executable);
    //PyRun_SimpleString("import sys; print(sys.path)");
    PLUTO_PY_interface_object = PLUTO_PY_CreateInterface(executable, logger);
    if(!PLUTO_PY_interface_object)
    {
        PLUTO_LoggerError(logger, "Error, unable to lead Interface Class from Script %s", executable);
        goto error;
    }
    //PyConfig_Clear(&config);
    if(buffer)
    {
        PLUTO_Free(buffer);
    }
    if(python_path_buffer.paths)
    {
        for(size_t i=0;i<N_PYTHON_PATHS;++i)
        {
            PLUTO_Free(python_path_buffer.paths[i]);
        }
    }
    PyConfig_Clear(&config);
    return true;

exception:
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
error:
    if(buffer)
    {
        PLUTO_Free(buffer);
    }
    if(python_path_buffer.paths)
    {
        for(size_t i=0;i<N_PYTHON_PATHS;++i)
        {
            PLUTO_Free(python_path_buffer.paths[i]);
        }
    }
    //PyConfig_Clear(&config);
    return false;
}

void PLUTO_DeinitializePython(void)
{
    // Teardown Object.
    if(PLUTO_PY_interface_object)
    {
        PyObject *result = PyObject_CallMethod(PLUTO_PY_interface_object, PLUTO_PYTHON_TEARDOWN_METHOD, NULL);
        if(!result)
        {
            PyErr_Print();
        }
        else
        {
            Py_DECREF(result);
        }
        Py_DECREF(PLUTO_PY_interface_object);
    }

    if (Py_FinalizeEx() < 0) 
    {
    }
}

PLUTO_ProcessorCallbackOutput_t PLUTO_PY_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args)
{
    //
    // https://docs.python.org/3/extending/extending.html#ownership-rules
    //
    PLUTO_PY_current_buffer = args;
    memcpy(args->output_buffer, args->input_buffer, args->input_buffer_size);
    
    PLUTO_PY_current_output_buffer.return_value = true;
    PLUTO_PY_current_output_buffer.output_size = 0;

    // Prepare Arguments
    PyObject *n_output_queues = PyLong_FromUnsignedLongLong(args->number_of_output_queues);
    PyObject *id = PyLong_FromLong(args->id);
    PyObject *event = PyLong_FromLong(args->event);
    PyObject *payload = PyUnicode_FromString(args->input_buffer);
    
    // Call Memberfunction on Object
    PyObject *method = PyUnicode_FromString(
        PLUTO_PYTHON_RUN_METHOD
    );
    PyObject *result = PyObject_CallMethodObjArgs(
        PLUTO_PY_interface_object, 
        method, 
        id, 
        event, 
        n_output_queues, 
        payload, 
        NULL
    );
    if(!result)
    {
        PLUTO_LoggerWarning(PLUTO_PY_logger, "NULL returned from Python-Callback.");
        PyErr_Print();
        PLUTO_PY_current_output_buffer.id = 0U;
        PLUTO_PY_current_output_buffer.event = 0U;
        PLUTO_PY_current_output_buffer.output_to_queues = 0U;
        PLUTO_PY_current_output_buffer.return_value = false;
        PLUTO_PY_current_output_buffer.output_size = 0LU;
    }
    else
    {
        const size_t strl_t = strlen("tuple");
        const size_t strl_d = strlen(Py_TYPE(result)->tp_name); 
        if(strl_t != strl_d)
        {
            PLUTO_LoggerWarning(PLUTO_PY_logger, "Python-Callback returned an unspecified Objtec: %s", Py_TYPE(result)->tp_name);
        }
        else
        {
            if(0 == memcmp("tuple", Py_TYPE(result)->tp_name, strl_t))
            {
                PLUTO_PY_current_output_buffer.return_value = true;
                if(4LU != PyTuple_Size(result))
                {
                    PLUTO_LoggerWarning(PLUTO_PY_logger, "Error, unable to parse Callback Result! Tuple has %u Elements.", (unsigned int)PyTuple_Size(result));
                    goto end;
                }
                //
                // These Items are borrowed because they are given to the caller from a Tuple.
                //
                PyObject *pyid = PyTuple_GetItem(result, 0LU);
                PyObject *pyevent = PyTuple_GetItem(result, 1LU);
                PyObject *pyoutputqueues = PyTuple_GetItem(result, 2LU);
                PyObject *pypayload = PyTuple_GetItem(result, 3LU);
                if(pyid && PyLong_Check(pyid)) 
                {
                    PLUTO_PY_current_output_buffer.id = PyLong_AsLong(pyid);
                }
                else
                {
                    PLUTO_PY_current_output_buffer.return_value = false;
                    goto error;
                }
                if(pyevent && PyLong_Check(pyevent))
                {
                    PLUTO_PY_current_output_buffer.event = PyLong_AsLong(pyevent);
                }
                else
                {
                    PLUTO_PY_current_output_buffer.return_value = false;
                    goto error;
                }
                if(pyoutputqueues && PyLong_Check(pyoutputqueues))
                {
                    PLUTO_PY_current_output_buffer.output_to_queues = PyLong_AsUnsignedLongLong(pyoutputqueues);
                }
                else
                {
                    PLUTO_PY_current_output_buffer.return_value = false;
                    goto error;
                }
                if(pypayload && PyUnicode_Check(pypayload))
                {
                    memcpy(
                        PLUTO_PY_current_buffer->output_buffer,
                        PyUnicode_AsUTF8(pypayload), 
                        strlen(
                            PyUnicode_AsUTF8(pypayload)
                        )
                    );
                    //PyObject *bytes = PyUnicode_AsEncodedString(payload, "utf-8", NULL);
                    //printf("Python Callback: %s, %lu\n", PyUnicode_AsUTF8(pypayload), strlen(PyUnicode_AsUTF8(pypayload)));
                    PLUTO_PY_current_output_buffer.output_size = strlen(PyUnicode_AsUTF8(pypayload));
                }
                else
                {
                    PLUTO_PY_current_output_buffer.return_value = false;
                    goto error;
                }
                error:
                PyErr_Print();
            }
            else
            {
                PLUTO_LoggerWarning(PLUTO_PY_logger, "Python-Callback returned an unspecified Objtec: %s", Py_TYPE(result)->tp_name);
            }
        }
        Py_DECREF(result);
    }
end:
    Py_DECREF(method);
    Py_DECREF(payload);
    Py_DECREF(event);
    Py_DECREF(id);
    Py_DECREF(n_output_queues);

    return PLUTO_PY_current_output_buffer;
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static bool PLUTO_PY_ReadPythonPathsFromEnv(const char *python_path, PLUTO_PY_PythonPath_t *paths)
{
    char buffer[8192];
    const size_t strl = strlen(python_path);
    if(strl > sizeof(buffer))
    {
        return false;
    }
    memcpy(buffer, python_path, strlen(python_path) + 1);
    paths->n_paths = 0;
    char *str = strtok(buffer, ";");
    while(NULL != str)
    {
        memcpy(paths->paths[paths->n_paths], str, strlen(str) + 1);
        paths->n_paths++;
        str = strtok(NULL, ";");
    }
    return true;
} 

//
// --------------------------------------------------------------------------------------------------------------------
//

//
// Don't call this Functions outside of PLUTO_PY_CraeteInterface()! 
//

static PyObject* PLUTO_PY_GetClass(const char *module_name, PLUTO_Logger_t logger)
{
    PLUTO_LoggerInfo(logger, "Try to import Modules %s", module_name);
    PyObject *module = PyImport_ImportModule(module_name);
    if(!module)
    {
        PLUTO_LoggerError(logger, "Unable to import Module!");
        goto error;
    }
    
    PyObject *dict = PyModule_GetDict(module);
    if(!dict)
    {
        PLUTO_LoggerError(logger, "Unable to get Module Dict!");
        goto error;
    }
    Py_DECREF(module);

    PyObject *class = PyDict_GetItemString(dict, PLUTO_PYTHON_INTERFACE_CLASS);
    if(!class)
    {
        PLUTO_LoggerError(logger, "Unable to get Class from Module Dict!");
        goto error;
    }
    Py_DECREF(dict);

    if(!PyCallable_Check(class))
    {
        PLUTO_LoggerError(logger, "Error, Class is not Callable...");
        goto error;
    }
    return class;

error:
    return NULL;
}

static PyObject* PLUTO_PY_CreateSetupArgs(int argc, char **argv)
{
    // Create Value for positional Argument 2 => "argv".
    PyObject *pValue;
    PyObject *pArgv = PyTuple_New(argc);
    for (int i = 0; i < argc; ++i) {
        pValue = PyUnicode_FromString(argv[i]);
        if (!pValue) {
            PyErr_Print();
            goto error;
        }
        /* pValue reference stolen here: */
        PyTuple_SetItem(pArgv, i, pValue);
    }
    return pArgv;

error:
    return NULL;
}

static PyObject* PLUTO_PY_CreateInterface(const char *path, PLUTO_Logger_t logger)
{
    gstate = PyGILState_Ensure();

    PyObject *class = PLUTO_PY_GetClass(path, logger); 
    if(!class)
    {
        PLUTO_LoggerError(logger, "Unable to find Interface Class.");
        goto error;
    }
    //
    // Initialize/Setup Object.
    //
    PyObject *object = PyObject_CallObject(class, NULL);
    Py_DECREF(class);
    
    int argc = 1;
    char *argv[2] =
    {
        "pluto_node",
        NULL
    };
    // Create Value for positional Argument 1 => "argc".
    PyObject *pArgc = PyLong_FromLong((long)argc);
    if(!pArgc)
    {
        Py_DECREF(object);
        goto error;
    }
    // Create Value for positional Argument 2 => "argv".
    PyObject *pArgv = PLUTO_PY_CreateSetupArgs(argc, argv);
    if(!pArgv)
    {
        Py_DECREF(object);
        Py_DECREF(pArgc);
        goto error;
    }
    // Create Pythonobject for calling the Method on our interface Object.
    PyObject *name = PyUnicode_FromString(PLUTO_PYTHON_SETUP_METHOD);
    if(!name)
    {   
        Py_DECREF(object);
        Py_DECREF(pArgc);
        Py_DECREF(pArgv);
        goto error;
    }
    // Call the Objects Method with "argc" and "argv".
    PyObject *result = PyObject_CallMethodObjArgs(object, name, pArgc, pArgv, NULL);
    Py_DECREF(name);
    Py_DECREF(pArgc);
    Py_DECREF(pArgv);
    if(!result)
    {
        PyErr_Print();
        Py_DECREF(object);
        object = NULL;
    }
    else
    {
        Py_DECREF(result);
    }
    
    PyGILState_Release(gstate);
    PLUTO_LoggerInfo(logger, "Python Initialization of Interface Object done...");
    return object;

error:
    PyErr_Print();
    PyGILState_Release(gstate);
    return NULL;
}
//
// --------------------------------------------------------------------------------------------------------------------
//

static PyObject* PLUTO_PythonCAPIRegisterFileObserver(PyObject *self, PyObject *args)
{
    (void)self;
    int filedescriptor;
    if(!PyArg_ParseTuple(args, "i", &filedescriptor))
    {
        PyErr_BadArgument();
        return NULL;
    }
    PLUTO_c_api.register_file_observer(filedescriptor);
    return Py_None;
}

static PyObject* PLUTO_PythonCAPIDeregisterFileObserver(PyObject *self, PyObject *args)
{
    (void)self;
    int filedescriptor;
    if(!PyArg_ParseTuple(args, "i", &filedescriptor))
    {
        PyErr_BadArgument();
        return NULL;
    }
    PLUTO_c_api.deregister_file_observer(filedescriptor);
    return Py_None;
}

static PyObject* PLUTO_PythonCAPIEmitEvent(PyObject *self, PyObject* args)
{
    (void)self;
    int id = -1;
    int event = -1;
    char *string = NULL;
    if(!PyArg_ParseTuple(args, "iis", &id, &event, &string))
    {
        return PyLong_FromLong(-1l);
    }
    const int c_api_result = PLUTO_c_api.emit_event(id, event, string, strlen(string) + 1);
    return PyLong_FromLong((long)c_api_result);
}

static PyObject* PLUTO_PythonInitPluto(void)
{
    //
    //  events
    //      |
    //      +-- observer
    //      |       |
    //      |       +-- signals
    //      |       +-- system
    //      |       +-- external
    //      |
    //      +-- emit
    //              |
    //              +-- external
    //
    static PyMethodDef pluto_module_methods[] = 
    {
        {
            "register_file_observer", 
            PLUTO_PythonCAPIRegisterFileObserver, 
            METH_VARARGS,
            "Observe the activity of a given Object. "
            "Pass a Filedescriptor to Pluto."
        },
        {
            "deregister_file_observer", 
            PLUTO_PythonCAPIDeregisterFileObserver,
            METH_VARARGS,
            "Observe the activity of a given Object. "
            "Pass a Filedescriptor to Pluto."
        },
        {
            "emit_event", 
            PLUTO_PythonCAPIEmitEvent, 
            METH_VARARGS,
            "Observe the activity of a given Object. "
            "Pass a Filedescriptor to Pluto."
        },
        {NULL, NULL, 0, NULL}
    };
    static struct PyModuleDef pluto_module = 
    {
        .m_base=PyModuleDef_HEAD_INIT,
        .m_name="pluto",
        .m_size=0,
        .m_methods=pluto_module_methods,
    };
    return PyModuleDef_Init(&pluto_module);
}

static int PLUTO_PythonCAPI_DefaultRegisterObserver(int filedescriptor)
{
    (void)filedescriptor;
    return -1;
}

static int PLUTO_PythonCAPI_DefaultDeregisterObserver(int filedescriptor)
{
    (void)filedescriptor;
    return -1;
}

static int PLUTO_PythonCAPI_DefaultEmitEvent(int id, int event, const char *payload, size_t nbytes)
{
    (void)id;
    (void)event;
    (void)payload;
    (void)nbytes;
    return -1;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
