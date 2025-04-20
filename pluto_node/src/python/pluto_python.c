#include <pluto/pluto_compile_time_switches.h>
#include "pluto/pluto_processor.h"
#include <pluto/python/pluto_python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Python.h>
#include <wchar.h>


#define PLUTO_PYTHON_INTERFACE_CLASS "Main"
#define PLUTO_PYTHON_SETUP_METHOD "setup"
#define PLUTO_PYTHON_TEARDOWN_METHOD "teardown"
#define PLUTO_PYTHON_RUN_METHOD "run"

typedef struct
{
    char **paths;
    size_t n_paths;
} PLUTO_PY_PythonPath_t;

static bool PLUTO_PY_ReadPythonPathsFromEnv(PLUTO_PY_PythonPath_t *paths); 
static PyObject* PyInit_emb_input(void);
static PyObject* PLUTO_PY_CreateInterface(void);

static PLUTO_ProcessorCallbackInput_t *PLUTO_PY_current_buffer;
static PLUTO_ProcessorCallbackOutput_t PLUTO_PY_current_output_buffer;
static PyObject *PLUTO_PY_interface_object = NULL;
PyGILState_STATE gstate;

bool PLUTO_InitializePython(void)
{
    PyStatus status;
    PyConfig config;
    
    PyImport_AppendInittab("pluto", &PyInit_emb_input);
    PyConfig_InitIsolatedConfig(&config);
    
    // TODO: Check if venvs are needed...
    //char *env = getenv("VIRTUAL_ENV");
    //
    char *buffer = malloc(4096);
    // TODO: Memory Management...
    PLUTO_PY_PythonPath_t python_path = {
        .paths=malloc(sizeof(char*) * 64),
        .n_paths=0
    };
    PLUTO_PY_ReadPythonPathsFromEnv(&python_path);

    for(size_t j=0;j<python_path.n_paths;++j)
    {
        snprintf(buffer, 4096, "%s", python_path.paths[j]); 
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
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        goto exception;
    }
    
    //PyRun_SimpleString("import sys; print(sys.path)");
    PLUTO_PY_interface_object = PLUTO_PY_CreateInterface();
    if(!PLUTO_PY_interface_object)
    {
        goto error;
    }
    //PyConfig_Clear(&config);
    return true;

exception:
    printf("Exception! %s\n", status.err_msg);
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

    PyObject *result = PyObject_CallMethod(PLUTO_PY_interface_object, PLUTO_PYTHON_RUN_METHOD, NULL);
    if(!result)
    {
        PyErr_Print();
        PLUTO_PY_current_output_buffer.return_value = false;
        PLUTO_PY_current_output_buffer.output_size = 0LU;
    }
    else
    {
        Py_DECREF(result);
    }
    return PLUTO_PY_current_output_buffer;
}

static PyObject* emb_input(PyObject *self, PyObject *args)
{
    (void)self;
    (void)args;
    return PyBytes_FromString(PLUTO_PY_current_buffer->input_buffer);
}

static PyObject* emb_output(PyObject *self, PyObject *args)
{
    // https://www.mit.edu/people/amliu/vrut/python/ext/parseTuple.html
    char *s;
    if(!PyArg_ParseTuple(args, "s", &s)) 
        return NULL;
    memcpy(PLUTO_PY_current_buffer->output_buffer, s, strlen(s));
    PLUTO_PY_current_output_buffer.output_size = strlen(s);
    PLUTO_PY_current_output_buffer.return_value = true;
    return self;
}

static PyMethodDef EmbMethods[] = {
    {"current_event", emb_input, METH_VARARGS,
     "Return the number of arguments received by the process."},
    {"output_result", emb_output, METH_VARARGS,
     "Return the number of arguments received by the process."},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef EmbModule = {
    PyModuleDef_HEAD_INIT, "pluto", NULL, -1, EmbMethods,
    NULL, NULL, NULL, NULL
};

static PyObject* PyInit_emb_input(void)
{
    return PyModule_Create(&EmbModule);
}
//
// --------------------------------------------------------------------------------------------------------------------
//

static bool PLUTO_PY_ReadPythonPathsFromEnv(PLUTO_PY_PythonPath_t *paths)
{
    char *env = getenv("PLUTO_PYTHON_PATH");
    char *str = strtok(env, ";");
    while(NULL != str)
    {
        paths->paths[paths->n_paths] = str;
        paths->n_paths++;
        str = strtok(env, ";");
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

static PyObject* PLUTO_PY_CreateInterface(void)
{
    gstate = PyGILState_Ensure();

    PyObject *class = PLUTO_PY_GetClass("main"); 
    if(!class)
    {
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
    PyObject *name = PyUnicode_InternFromString(PLUTO_PYTHON_SETUP_METHOD);
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
    return object;

error:
    PyGILState_Release(gstate);
    return NULL;
}
//
// --------------------------------------------------------------------------------------------------------------------
//
