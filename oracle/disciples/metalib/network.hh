//
// ---------- header ----------------------------------------------------------
//
// project       oracle/disciples/metalib
//
// license       infinit
//
// author        Raphaël Londeix   [Thu 01 Mar 2012 04:03:37 PM CET]
//

#ifndef ORACLE_DISCIPLES_METALIB_NETWORK_HH
# define ORACLE_DISCIPLES_METALIB_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Python.h>

//
// ---------- functions -------------------------------------------------------
//

extern "C" PyObject* metalib_check_root_block_signature(PyObject* self,
                                                        PyObject* args);


extern "C" PyObject* metalib_generate_network_descriptor(PyObject* self,
                                                         PyObject* args);

#endif /* ! NETWORK_HH */


