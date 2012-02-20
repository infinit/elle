//
// ---------- header ----------------------------------------------------------
//
// project       oracle/disciples/metalib
//
// license       infinit
//
// author        Raphael Londeix   [Mon 20 Feb 2012 03:14:17 PM CET]
//

#ifndef ORACLE_DISCIPLES_METALIB_IDENTITY_HH
# define ORACLE_DISCIPLES_METALIB_IDENTITY_HH

# include <Python.h>

extern "C"
{
  PyObject* metalib_generate_identity(PyObject* self, PyObject* args);
}

#endif /* ! IDENTITY_HH */


