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
    "Generate an identity.\n"
    "\n"
    ":param login: The user unique id (its email)\n"
    ":param password: The user password\n"
    ":param authority_file: The path to the authority file\n"
    ":param authority_password: Password to decrypt the authority file\n"
    ":rtype: A tuple of two string\n"
    ":return: a pair of strings: the whole identity and the public part of the identity\n"
  },
  {
    "generate_passport",
    &metalib_generate_passport,
    METH_VARARGS,
    "Generate a passport.\n"
    "\n"
    ":param authority_file: The path to the authority file\n"
    ":param authority_password: Password to decrypt the authority file\n"
    ":rtype: String\n"
    ":return: The encoded passport\n"
  },
  {NULL, NULL, 0, NULL}        /* Sentinel */
};


char const* const _metalib_doc =
  "User friendly interface for infinit files\n"
;

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

  PyObject* module = Py_InitModule3(
      METALIB_MOD_NAME,
      _metalib_methods,
      _metalib_doc
  );

  char error_name[] = "metalib.MetaError";

  metalib_MetaError = PyErr_NewException(error_name, nullptr, nullptr);
  Py_INCREF(metalib_MetaError);
  PyModule_AddObject(module, error_name, metalib_MetaError);
}
