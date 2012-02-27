//
// ---------- header ----------------------------------------------------------
//
// project       oracle/disciples/metalib
//
// license       infinit
//
// author        Raphael Londeix   [Mon 20 Feb 2012 03:17:55 PM CET]
//

#include "elle/cryptography/KeyPair.hh"
#include "elle/io/Path.hh"
#include "elle/core/String.hh"

#include "lune/Identity.hh"

// XXX When Qt is out, remove this
#ifdef slots
# undef slots
#endif
#ifdef slot
# undef slot
#endif


#include "metalib.hh"
#include "identity.hh"


///
/// this method creates a new user by generating a new key pair and
/// storing a user block.
///
static lune::Identity create_identity(elle::String const& authority_file,
                                      elle::String const& authority_password,
                                      elle::String const& login,
                                      elle::String const& password)
{
  elle::KeyPair       pair;
  lune::Authority     authority;
  elle::Path          authority_path;
  lune::Identity      identity;

  // check the argument.
  if (login.empty() == true)
    throw std::runtime_error("unable to create a user without a user name");

  if (authority_path.Create(authority_file) == elle::StatusError)
    throw std::runtime_error("unable to create authority path");

  // load the authority file
  if (authority.Load(authority_path) == elle::StatusError)
    throw std::runtime_error("unable to load the authority file");

  // decrypt the authority.
  if (authority.Decrypt(authority_password) == elle::StatusError)
    throw std::runtime_error("unable to decrypt the authority");

  // generate a key pair.
  if (pair.Generate() == elle::StatusError)
    throw std::runtime_error("unable to generate the key pair");

  // create the identity.
  if (identity.Create(login, pair) == elle::StatusError)
    throw std::runtime_error("unable to create the identity");

  // encrypt the identity.
  if (identity.Encrypt(password) == elle::StatusError)
    throw std::runtime_error("unable to encrypt the identity");

  // seal the identity.
  if (identity.Seal(authority) == elle::StatusError)
    throw std::runtime_error("unable to seal the identity");

  return identity;
}

extern "C" PyObject* metalib_generate_identity(PyObject* self, PyObject* args)
{
  char const* login = nullptr,
            * password = nullptr,
            * auth_path = nullptr,
            * auth_password = nullptr;
  PyObject* ret = nullptr;

  if (!PyArg_ParseTuple(args, "ssss:generate_identity",
                        &login, &password, &auth_path, &auth_password))
    return nullptr;

  if (!login || !password || !auth_path || !auth_password)
    return nullptr;


  PyThreadState *_save;
  _save = PyEval_SaveThread();

  try
    {
      auto identity = create_identity(auth_path, auth_password, login, password);
      elle::String all, pub;
      bool res = (
          identity.Save(all) != elle::StatusError &&
          identity.pair.k.Save(pub) != elle::StatusError
      );
      // WARNING: restore state before setting exception !
      PyEval_RestoreThread(_save);

      if (res)
        {
          ret = Py_BuildValue("(ss)", all.c_str(), pub.c_str());
        }
      else
        {
          PyErr_SetString(
              metalib_MetaError,
              "Cannot convert the identity to string"
          );
        }
    }
  catch (std::exception const& err)
    {
      // WARNING: restore state before setting exception !
      PyEval_RestoreThread(_save);
      //show();
      char const* error_string = err.what();
      PyErr_SetString(metalib_MetaError, error_string);
    }

  return ret;
}

