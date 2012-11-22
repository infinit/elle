#include "metalib.hh"

#include <lune/Identity.hh>

#include <elle/io/Path.hh>
#include <elle/types.hh>

#include <cryptography/random.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

#include <elle/Passport.hh>
#include <elle/Authority.hh>

// XXX When Qt is out, remove this
#ifdef slots
# undef slots
#endif
#ifdef slot
# undef slot
#endif

#include "passport.hh"

//
// ---------- functions  ------------------------------------------------------
//

static elle::Passport create_passport(elle::String const& id,
                                      elle::String const& name,
                                      elle::String const& user_id,
                                      elle::String const& authority_file,
                                      elle::String const& authority_password)
{
  elle::io::Path      authority_path;

  if (authority_path.Create(authority_file) == elle::Status::Error)
    throw std::runtime_error("unable to create authority path");

  // Load the authority file.
  elle::Authority authority(authority_path);

  // decrypt the authority.
  if (authority.Decrypt(authority_password) == elle::Status::Error)
    throw std::runtime_error("unable to decrypt the authority");

  lune::Identity identity;
  identity.load(user_id);

  elle::Passport passport{
      id, name, identity.pair.K(), authority
  };

  return passport;
}

extern "C"
PyObject*
metalib_generate_passport(PyObject*, PyObject* args)
{
  PyObject* ret = nullptr;
  char const* id = nullptr,
            * name = nullptr,
            * user_id = nullptr,
            * authority_file = nullptr,
            * authority_password = nullptr;
  if (!PyArg_ParseTuple(args, "sssss:generate_passport",
                        &id, &name, &user_id, &authority_file, &authority_password))
    return nullptr;
  if (!id || !authority_file || !name || !user_id || !authority_password)
    return nullptr;

  Py_BEGIN_ALLOW_THREADS;

  try
    {
      auto passport = create_passport(
          id, name, user_id, authority_file, authority_password
      );
      elle::String passport_string;
      if (passport.Save(passport_string) != elle::Status::Error)
        {
          ret = Py_BuildValue("s", passport_string.c_str());
        }
      else
        {
          PyErr_SetString(
              metalib_MetaError,
              "Cannot convert the passport to string"
          );
        }
    }
  catch(std::exception const& err)
    {
      PyErr_SetString(metalib_MetaError, err.what());
    }

  Py_END_ALLOW_THREADS;

  return ret;
}
