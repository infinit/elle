#ifndef ORACLE_DISCIPLES_METALIB_NETWORK_HH
# define ORACLE_DISCIPLES_METALIB_NETWORK_HH

# include <Python.h>

extern "C"
PyObject*
metalib_check_root_directory_signature(PyObject* self,
                                       PyObject* args);

extern "C"
PyObject*
metalib_generate_network_descriptor(PyObject* self,
                                    PyObject* args);

#endif /* ! NETWORK_HH */
