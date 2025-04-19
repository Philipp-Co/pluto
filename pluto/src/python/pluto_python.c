#include "pluto/pluto_processor.h"
#include <pluto/python/pluto_python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Python.h>
#include <wchar.h>

static PLUTO_ProcessorCallbackInput_t *PLUTO_PY_current_buffer;
static PLUTO_ProcessorCallbackOutput_t PLUTO_PY_current_output_buffer;

static PLUTO_ProcessorCallbackInput_t *PLUTO_PY_current_buffer;
static PyObject* PyInit_emb_input(void);
static PyObject* PLUTO_PY_CreateInterface(void);

static PyObject *PLUTO_PY_interface_object = NULL;

bool PLUTO_InitializePython(void)
{
    PyStatus status;
    PyConfig config;
    
    PyImport_AppendInittab("pluto", &PyInit_emb_input);
    PyConfig_InitIsolatedConfig(&config);
    
    // TODO: Check if venvs are needed...
    //char *env = getenv("VIRTUAL_ENV");
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        goto exception;
    }
    
    PLUTO_PY_interface_object = PLUTO_PY_CreateInterface();
    if(!PLUTO_PY_interface_object)
    {
        goto exception;
    }
    PyConfig_Clear(&config);
    return true;

exception:
    printf("Exception! %s\n", status.err_msg);
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
    return false;
}

void PLUTO_DeinitializePython(void)
{
    Py_DECREF(PLUTO_PY_interface_object);
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

    PyObject_CallMethod(PLUTO_PY_interface_object, "run", NULL);
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

static PyObject* PLUTO_PY_CreateInterface(void)
{
    PyObject *module = PyImport_ImportModule("main");
    if(!module)
    {
        fprintf(stderr, "Unable to import Module!\n");
        return NULL;
    }
    
    PyObject *dict = PyModule_GetDict(module);
    if(!dict)
    {
        fprintf(stderr, "Unable to get Module Dict!\n");
        return NULL;
    }
    Py_DECREF(module);

    PyObject *class = PyDict_GetItemString(dict, "Test");
    if(!class)
    {
        fprintf(stderr, "Unable to get Class from Module Dict!\n");
        return NULL;
    }
    Py_DECREF(dict);

    if(!PyCallable_Check(class))
    {
        return NULL;
    }

    PyObject *object = PyObject_CallObject(class, NULL);
    Py_DECREF(class);
    return object;
}
