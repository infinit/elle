#include "metalib.hh"

#include <elle/io/Path.hh>
#include <elle/types.hh>

#include <cryptography/random.hh>
#include <cryptography/PublicKey.hh>
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
                                      elle::String const& user_pubkey,
                                      elle::String const& authority_file,
                                      elle::String const& authority_password)
{
  // Load the authority file.
  elle::Authority authority{elle::io::Path{authority_file}};

  // decrypt the authority.
  if (authority.Decrypt(authority_password) == elle::Status::Error)
    throw std::runtime_error("unable to decrypt the authority");

  cryptography::PublicKey pubkey{};
  pubkey.Restore(user_pubkey);

  elle::Passport passport{
      id, name, pubkey, authority
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
            * pubkey = nullptr,
            * authority_file = nullptr,
            * authority_password = nullptr;
  if (!PyArg_ParseTuple(args, "sssss:generate_passport",
                        &id, &name, &pubkey, &authority_file, &authority_password))
    return nullptr;
  if (!id || !authority_file || !name || !pubkey || !authority_password)
    return nullptr;

  Py_BEGIN_ALLOW_THREADS;

  try
    {
      auto passport = create_passport(
          id, name, pubkey, authority_file, authority_password
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
      std::cerr << err.what() << std::endl;
      PyErr_SetString(metalib_MetaError, err.what());
    }

  Py_END_ALLOW_THREADS;

  return ret;
}
