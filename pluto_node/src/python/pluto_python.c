
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_compile_time_switches.h>
#include "pluto/pluto_processor.h"
#include <pluto/python/pluto_python.h>

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
} PLUTO_PY_PythonPath_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

static bool PLUTO_PY_ReadPythonPathsFromEnv(const char *python_path, PLUTO_PY_PythonPath_t *paths);
//static PyObject* PyInit_emb_input(void);
static PyObject* PLUTO_PY_CreateInterface(const char *path);

//
// --------------------------------------------------------------------------------------------------------------------
//

static PLUTO_ProcessorCallbackInput_t *PLUTO_PY_current_buffer;
static PLUTO_ProcessorCallbackOutput_t PLUTO_PY_current_output_buffer;
static PyObject *PLUTO_PY_interface_object = NULL;
PyGILState_STATE gstate;

//
// --------------------------------------------------------------------------------------------------------------------
//

bool PLUTO_InitializePython(const char *python_path, const char *executable)
{
    PyStatus status;
    PyConfig config;
    
    printf("Start Python configuration.\n");
    //PyImport_AppendInittab("pluto", &PyInit_emb_input);
    PyConfig_InitIsolatedConfig(&config);
    
    // TODO: Check if venvs are needed...
    //char *env = getenv("VIRTUAL_ENV");
    //
    char *buffer = malloc(4096);
    if(!buffer) return false;
    // TODO: Memory Management...
    PLUTO_PY_PythonPath_t python_path_buffer = {
        .paths=malloc(sizeof(char*) * 128),
        .n_paths=0
    };
    for(size_t i=0LU;i<128;++i)
    {
        python_path_buffer.paths[i] = malloc(256);
        memset(python_path_buffer.paths[i], '\0', 256);
    }

    printf("Read Python ENV \"%s\".\n", python_path);
    PLUTO_PY_ReadPythonPathsFromEnv(python_path, &python_path_buffer);
    printf("Initialize Python Interpreter with:\n");
    for(size_t i=0;i<python_path_buffer.n_paths;++i)
        printf("  %s\n", python_path_buffer.paths[i]);

    for(size_t j=0;j<python_path_buffer.n_paths;++j)
    {
        snprintf(buffer, 4096, "%s", python_path_buffer.paths[j]); 
        wchar_t *wcstr = malloc(sizeof(wchar_t) * (strlen(buffer) + 1));
        for(size_t i=0;i<(strlen(buffer)+1);++i)
        {
            wcstr[i] = L'\0';
        } 
        mbstowcs(wcstr, buffer, strlen(buffer));
        PyWideStringList_Append(&config.module_search_paths, wcstr);
    }
    config.module_search_paths_set = 1;
    //
    printf("Initialize Python Interpreter from Config.\n");
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        goto exception;
    }
    
    printf("Read Interface from Script.\n");
    printf("  %s\n", executable);
    //PyRun_SimpleString("import sys; print(sys.path)");
    PLUTO_PY_interface_object = PLUTO_PY_CreateInterface(executable);
    if(!PLUTO_PY_interface_object)
    {
        printf("Error, unable to load Interface from Script %s\n", executable);
        goto error;
    }
    //PyConfig_Clear(&config);
    return true;

exception:
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
error:
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
    PLUTO_PY_current_buffer = args;
    memcpy(args->output_buffer, args->input_buffer, args->input_buffer_size);
    
    PLUTO_PY_current_output_buffer.return_value = true;
    PLUTO_PY_current_output_buffer.output_size = 0;

    // Prepare Arguments
    PyObject *id = PyLong_FromLong(args->id);
    PyObject *event = PyLong_FromLong(args->event);
    PyObject *payload = PyUnicode_FromString(args->input_buffer);
    
    // Call Memberfunction on Object
    PyObject *method = PyUnicode_FromString(PLUTO_PYTHON_RUN_METHOD);
    PyObject *result = PyObject_CallMethodObjArgs(PLUTO_PY_interface_object, method, id, event, payload, NULL);
    if(!result)
    {
        PyErr_Print();
        PLUTO_PY_current_output_buffer.id = 0U;
        PLUTO_PY_current_output_buffer.event = 0U;
        PLUTO_PY_current_output_buffer.return_value = false;
        PLUTO_PY_current_output_buffer.output_size = 0LU;
    }
    else
    {
        const size_t strl_t = strlen("tuple");
        const size_t strl_d = strlen(Py_TYPE(result)->tp_name); 
        if(strl_t != strl_d)
        {
            printf("No Tuple!\n");
        }
        else
        {
            if(0 == memcmp("tuple", Py_TYPE(result)->tp_name, strl_t))
            {
                PLUTO_PY_current_output_buffer.return_value = true;
                PyObject *pyid = PyTuple_GetItem(result, 0LU);
                PyObject *pyevent = PyTuple_GetItem(result, 1LU);
                PyObject *pypayload = PyTuple_GetItem(result, 2LU);
                if(pyid && PyLong_Check(pyid)) 
                {
                    PLUTO_PY_current_output_buffer.id = PyLong_AsLong(pyid);
                    Py_DECREF(pyid);
                }
                else
                {
                    PLUTO_PY_current_output_buffer.return_value = false;
                    goto error;
                }
                if(pyevent && PyLong_Check(pyevent))
                {
                    PLUTO_PY_current_output_buffer.event = PyLong_AsLong(pyevent);
                    Py_DECREF(pyevent);
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
                    PLUTO_PY_current_output_buffer.output_size = strlen(PyUnicode_AsUTF8(payload));
                    Py_DECREF(pypayload);
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
                printf("Result is no Tuple!\n");
            }
        }
        Py_DECREF(result);
    }
    Py_DECREF(method);
    Py_DECREF(payload);
    Py_DECREF(event);
    Py_DECREF(id);

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

static PyObject* PLUTO_PY_GetClass(const char *module_name)
{
    PyObject *module = PyImport_ImportModule(module_name);
    if(!module)
    {
        fprintf(stderr, "Unable to import Module!\n");
        goto error;
    }
    
    PyObject *dict = PyModule_GetDict(module);
    if(!dict)
    {
        fprintf(stderr, "Unable to get Module Dict!\n");
        goto error;
    }
    Py_DECREF(module);

    PyObject *class = PyDict_GetItemString(dict, PLUTO_PYTHON_INTERFACE_CLASS);
    if(!class)
    {
        fprintf(stderr, "Unable to get Class from Module Dict!\n");
        goto error;
    }
    Py_DECREF(dict);

    if(!PyCallable_Check(class))
    {
        fprintf(stderr, "Error, Class is not Callable...\n");
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
    printf("Python argv created..\n");
    return pArgv;

error:
    return NULL;
}

static PyObject* PLUTO_PY_CreateInterface(const char *path)
{
    gstate = PyGILState_Ensure();

    PyObject *class = PLUTO_PY_GetClass(path); 
    if(!class)
    {
        printf("Unable to find Interface Class.\n");
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
    printf("Call setup...\n");
    PyObject *result = PyObject_CallMethodObjArgs(object, name, pArgc, pArgv, NULL);
    printf("after Setup, %p.\n", (void*)result);
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
    printf("Python Initialization of Interface Object done...\n");
    return object;

error:
    PyErr_Print();
    PyGILState_Release(gstate);
    return NULL;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
