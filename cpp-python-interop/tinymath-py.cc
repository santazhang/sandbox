#include <stdio.h>

#include <Python.h>

#include "tinymath.h"

using namespace tinymath;

static PyObject* tinymath_gcd(PyObject* self, PyObject* args) {
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

static PyMethodDef TinyMathMethods[] = {
    {"gcd", tinymath_gcd, METH_VARARGS, "get gcd value of a and b"},
    {NULL, NULL, 0, NULL}
};

extern "C" {

PyMODINIT_FUNC inittinymathpy(void) {
    PyObject* m;
    m = Py_InitModule("tinymathpy", TinyMathMethods);
    if (m == NULL)
        return;
}

}
