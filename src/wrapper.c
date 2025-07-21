#include <Python.h>
#include "lnsocket.h"  // Header from the static lib

static PyObject* create(PyObject* self, PyObject* args) {
    lnsocket_create();  // Function from the static library
    Py_RETURN_NONE;
}

static PyMethodDef Methods[] = {
    {"create", create, METH_NOARGS, "Call lnsocket_create from static lib"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "wrapper",
    NULL,
    -1,
    Methods
};

PyMODINIT_FUNC PyInit_wrapper(void) {
    return PyModule_Create(&moduledef);
}

