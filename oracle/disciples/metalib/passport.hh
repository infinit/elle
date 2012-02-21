//
// ---------- header ----------------------------------------------------------
//
// project       oracle/disciples/metalib
//
// license       infinit
//
// author        Raphaël Londeix   [Tue 21 Feb 2012 11:35:40 AM CET]
//

#ifndef ORACLE_DISCIPLES_METALIB_PASSPORT_HH
# define ORACLE_DISCIPLES_METALIB_PASSPORT_HH

//
// ---------- includes --------------------------------------------------------
//

# include <Python.h>

//
// ---------- functions -------------------------------------------------------
//

extern "C"
{
  PyObject* metalib_generate_passport(PyObject* self, PyObject* args);
}

#endif /* ! PASSPORT_HH */
