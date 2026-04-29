#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdlib.h>

#include <dbeaver-creds.h>

static PyObject *py_get_dbeaver_credentials(PyObject *self, PyObject *args, PyObject *kwargs) {
    (void)self;
    static char *kwlist[] = {"path", nullptr};
    PyObject *path_obj = Py_None;
    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, "|O:get_dbeaver_credentials", kwlist, &path_obj)) {
        return nullptr;
    }
    PyObject *path_bytes = nullptr;
    const char *path = nullptr;
    if (path_obj != Py_None) {
        if (!PyUnicode_FSConverter(path_obj, &path_bytes)) {
            return nullptr;
        }
        path = PyBytes_AS_STRING(path_bytes);
    }
    char *json;
    Py_BEGIN_ALLOW_THREADS;
    json = get_dbeaver_credentials(path);
    Py_END_ALLOW_THREADS;
    Py_XDECREF(path_bytes);
    if (!json) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to read DBeaver credentials.");
        return nullptr;
    }
    PyObject *result = PyUnicode_FromString(json);
    free(json);
    return result;
}

static PyMethodDef methods[] = {
    {"get_dbeaver_credentials",
     (PyCFunction)(void (*)(void))py_get_dbeaver_credentials,
     METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR(
         "get_dbeaver_credentials(path=None) -> str\n\n"
         "Read and decrypt DBeaver's credentials-config.json. If path is None, "
         "the platform-default location is used. Returns the JSON payload as a str.")},
    {nullptr, nullptr, 0, nullptr},
};

static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "dbeaver_creds._dbeaver_creds",
    PyDoc_STR("Native bindings for dbeaver-creds."),
    -1,
    methods,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

PyMODINIT_FUNC PyInit__dbeaver_creds(void) {
    return PyModule_Create(&module_def);
}
