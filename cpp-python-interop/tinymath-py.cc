#include <stdio.h>
#include <pthread.h>
#include <string>
#include <Python.h>

#include "tinymath.h"

using namespace tinymath;

struct GILHelper {
  PyGILState_STATE gstate;
  GILHelper() {
    gstate = PyGILState_Ensure();
  }

  ~GILHelper() {
    PyGILState_Release(gstate);
  }
};

static PyObject* tinymath_gcd(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    int a;
    int b;
    int result;
    if (!PyArg_ParseTuple(args, "ii", &a, &b))
        return NULL;
    TinyMath tm;
    printf("c++: a = %d\n", a);
    printf("c+=: b = %d\n", b);
    result = tm.gcd(a, b);
    printf("c++: gcd = %d\n", result);
    return Py_BuildValue("i", result);
}

static void* mt_callback(void* arg) {
    {
        GILHelper gil_helper;
        PyObject* func = (PyObject *) arg;

        Py_XINCREF(func);
        std::string s = "from C";
        PyObject* params = Py_BuildValue("(s)", &s[0]);
        PyObject_CallObject(func, params);
        Py_XDECREF(func);
    }
    pthread_exit(NULL);
    return NULL;
}

static PyObject* tinymath_mt_callback(PyObject* self, PyObject* args) {
    GILHelper gil_helper;

    PyObject* func;
    if (!PyArg_ParseTuple(args, "O", &func))
        return NULL;

    Py_XINCREF(func);

    for (int i = 0; i < 100; i++) {
        pthread_t th;
        pthread_create(&th, NULL, mt_callback, func);
    }

    Py_RETURN_NONE;
}

static PyMethodDef TinyMathMethods[] = {
    {"gcd", tinymath_gcd, METH_VARARGS, "get gcd value of a and b"},
    {"mt_callback", tinymath_mt_callback, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC inittinymathpy(void) {
    PyEval_InitThreads();
    GILHelper gil_helper;

    PyObject* m;
    m = Py_InitModule("tinymathpy", TinyMathMethods);
    if (m == NULL)
        return;
}

