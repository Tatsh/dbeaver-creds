#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "compat.h"

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
    enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_OK;
    Py_BEGIN_ALLOW_THREADS;
    json = get_dbeaver_credentials(path, &err);
    Py_END_ALLOW_THREADS;
    Py_XDECREF(path_bytes);
    if (!json) {
        const char *msg;
        PyObject *exc_type = PyExc_RuntimeError;
        switch (err) {
        case DBEAVER_CREDENTIALS_PATH_UNAVAILABLE:
            msg = "Could not determine credentials path.";
            break;
        case DBEAVER_CREDENTIALS_FILE_READ_FAILED:
            msg = "credentials-config.json file not found or could not be read.";
            exc_type = PyExc_FileNotFoundError;
            break;
        case DBEAVER_CREDENTIALS_INVALID_CIPHERTEXT:
            msg = "Invalid ciphertext length.";
            exc_type = PyExc_ValueError;
            break;
        case DBEAVER_CREDENTIALS_DECRYPTION_FAILED:
            msg = "Decryption failed.";
            exc_type = PyExc_ValueError;
            break;
        case DBEAVER_CREDENTIALS_EMPTY_PAYLOAD:
            msg = "Decryption produced no payload.";
            exc_type = PyExc_ValueError;
            break;
        case DBEAVER_CREDENTIALS_OUT_OF_MEMORY:
            PyErr_NoMemory();
            return nullptr;
        case DBEAVER_CREDENTIALS_OK:
        default:
            msg = "Failed to read DBeaver credentials.";
            break;
        }
        PyErr_SetString(exc_type, msg);
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
     PyDoc_STR("get_dbeaver_credentials(path: str | os.PathLike[Any] | None = None) -> str\n\n"
               "Read and decrypt DBeaver's credentials-config.json.\n\n"
               "Parameters\n"
               "----------\n"
               "path : str | os.PathLike[Any] | None\n"
               "    Filesystem path to the credentials file. If None, the\n"
               "    platform-default location is used.\n\n"
               "Returns\n"
               "-------\n"
               "str\n"
               "    The decrypted JSON payload.\n\n"
               "Raises\n"
               "------\n"
               "FileNotFoundError\n"
               "    The credentials file could not be opened or fully read.\n"
               "ValueError\n"
               "    The ciphertext was malformed, decryption failed, or the\n"
               "    payload was empty after the IV prefix.\n"
               "MemoryError\n"
               "    A heap allocation failed while assembling the result.\n"
               "RuntimeError\n"
               "    The platform-default credentials path could not be\n"
               "    determined.")},
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
