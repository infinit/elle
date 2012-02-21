//
// ---------- header ----------------------------------------------------------
//
// project       oracle/disciples/metalib
//
// license       infinit
//
// author        Raphael Londeix   [Mon 20 Feb 2012 02:57:13 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include "lune/Lune.hh"
#include "elle/Elle.hh"

#include "identity.hh"
#include "passport.hh"
#include "metalib.hh"

/// metalib error
PyObject* metalib_MetaError = nullptr;

/// metalib methods
static PyMethodDef _metalib_methods[] = {
  {
    "generate_identity",
    &metalib_generate_identity,
    METH_VARARGS,
    "Generate an identity."
  },
  {
    "generate_passport",
    &metalib_generate_passport,
    METH_VARARGS,
    "Generate a passport."
  },
  {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC initmetalib(void)
{
  if (elle::Elle::Initialize() == elle::StatusError)
    {
      std::cerr << "Cannot initialize elle\n";
      return;
    }
  if (lune::Lune::Initialize() == elle::StatusError)
    {
      std::cerr << "Cannot initialize lune\n";
      return;
    }

  PyObject* module = Py_InitModule(METALIB_MOD_NAME, _metalib_methods);

  char error_name[] = "metalib.MetaError";

  metalib_MetaError = PyErr_NewException(error_name, nullptr, nullptr);
  Py_INCREF(metalib_MetaError);
  PyModule_AddObject(module, error_name, metalib_MetaError);
}
