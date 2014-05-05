/**
 * Set process titles in Python programs
 *
 * Based on work by Eugene A. Lisitsky
 *
 * Copyright (C) 2009  Ludvig Ericson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* This makes the "s#" format for PyArg_ParseTuple and such take a Py_ssize_t
 * instead of an int or whatever. */
#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* Py_ssize_t appeared in Python 2.5. */
#ifndef PY_SSIZE_T_MAX
typedef ssize_t Py_ssize_t;
#endif

#ifdef __linux
#include <sys/prctl.h>
#endif

/* This is actually not an exposed Python API, but it's been there since the
 * early days and won't go away. */
void Py_GetArgcArgv(int *argc, char ***argv);

static PyObject *procname_getprocname(PyObject *self, PyObject *args) {
    int argc;
    char **argv;
    Py_GetArgcArgv(&argc, &argv);
    return Py_BuildValue("s", *argv);
};

static PyObject *procname_setprocname(PyObject *self, PyObject *args) {
    int argc, argn;
    char **argv;
    char *name;
    Py_ssize_t buflen;

    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    Py_GetArgcArgv(&argc, &argv);
    for (argn = buflen = 0; argn < argc; argn++) {
        buflen += (Py_ssize_t)strlen(argv[argn]) + 1;
    }
    strncpy(*argv, name, (size_t)buflen - 1);
    if (buflen > 0) {
        argv[0][buflen - 1]= '\0';
    }

#ifdef __linux
    /* Use the much nicer prctl API where possible (GNU/Linux.)
       Note: Rather than overwriting argv as above, this sets task_struct.comm which is fixed at 16 bytes.
       Not quite the same!
       prctl() with PR_SET_MM and PR_SET_MM_ARG_START/PR_SET_MM_ARG_END could work, but would require
       `setcap 'CAP_SYS_RESOURCE=+ep'`. */
    if (prctl(PR_SET_NAME, name, 0, 0, 0)) {
        PyErr_SetFromErrno(PyExc_OSError);
    }
#endif

    Py_RETURN_NONE;
};

static PyMethodDef procname_methods[] = {
    {"getprocname", procname_getprocname, METH_VARARGS,
        "getprocname() -> current process name\n"},
    {"setprocname", procname_setprocname, METH_VARARGS,
        "setprocname(name) -- set process name\n"},
    {NULL, NULL, 0, NULL}
};

PyDoc_STRVAR(procname__doc__, "Module for setting/getting process name");

#if PY_VERSION_HEX >= 0x03000000
static struct PyModuleDef procname_module_def = {
    PyModuleDef_HEAD_INIT,
    "procname",
    procname__doc__,
    -1,
    procname_methods,
    NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_procname(void) {
    return PyModule_Create(&procname_module_def);
}
#else  /* Python 2.x */
PyMODINIT_FUNC initprocname(void) {
    Py_InitModule3("procname", procname_methods, procname__doc__);
}
#endif
