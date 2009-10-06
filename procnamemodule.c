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

#include <Python.h>
#if defined(_GNU_SOURCE) && !defined(_DARWIN_C_SOURCE)
#  include <sys/prctl.h>
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
    char *arg0, *arg, *name;
    Py_ssize_t buflen, name_sz;

    if (!PyArg_ParseTuple(args, "s#", &name, &name_sz)) {
        return NULL;
    }

    Py_GetArgcArgv(&argc, &argv);
    arg0 = *argv;
    for (argn = buflen = 0, arg = argv[argn]; argn < argc; argn++) {
        buflen += (Py_ssize_t)strlen(arg);
    }
    name_sz = (buflen < name_sz) ? buflen : name_sz;
    strncpy(arg0, name, (size_t)name_sz);
    memset(arg0 + name_sz, 0, strlen(arg0 + name_sz));

#if defined(_GNU_SOURCE) && !defined(_DARWIN_C_SOURCE)
    /* Use the much nicer prctl API where possible (GNU/Linux.) */
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

PyMODINIT_FUNC initprocname(void) {
    Py_InitModule3("procname", procname_methods, procname__doc__);
}
