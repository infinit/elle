//
// ---------- header ----------------------------------------------------------
//
// project       @FIXME@
//
// license       infinit
//
// author        Raphaël Londeix   [Tue 21 Feb 2012 11:36:47 AM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include "elle/io/Path.hh"
#include <elle/types.hh>

#include "lune/Passport.hh"

// XXX When Qt is out, remove this
#ifdef slots
# undef slots
#endif
#ifdef slot
# undef slot
#endif

#include "metalib.hh"
#include "passport.hh"

//
// ---------- functions  ------------------------------------------------------
//

static lune::Passport create_passport(elle::String const& id,
                                      elle::String const& authority_file,
                                      elle::String const& authority_password)
{
  lune::Authority     authority;
  elle::Path          authority_path;
  lune::Passport      passport;

  if (authority_path.Create(authority_file) == elle::Status::Error)
    throw std::runtime_error("unable to create authority path");

  // load the authority file
  if (authority.Load(authority_path) == elle::Status::Error)
    throw std::runtime_error("unable to load the authority file");

  // decrypt the authority.
  if (authority.Decrypt(authority_password) == elle::Status::Error)
    throw std::runtime_error("unable to decrypt the authority");

  //
  // create the passport.
  //
  {
    hole::Label               label;
    elle::Region              region;

    // generate a random region.
    if (elle::Random::Generate(region) == elle::Status::Error)
      throw std::runtime_error("unable to generate a random region");

    // create a label.
    if (label.Create(region) == elle::Status::Error)
      throw std::runtime_error("unable to create a label");

    // create the passport.
    if (passport.Create(label, id) == elle::Status::Error)
      throw std::runtime_error("unable to create the passport");

    // seal the passport.
    if (passport.Seal(authority) == elle::Status::Error)
      throw std::runtime_error("unable to seal the passport");
  }

  return passport;
}

extern "C" PyObject* metalib_generate_passport(PyObject* self, PyObject* args)
{
  PyObject* ret = nullptr;
  char const* id = nullptr,
            * authority_file = nullptr,
            * authority_password = nullptr;
  if (!PyArg_ParseTuple(args, "sss:generate_passport",
                        &id, &authority_file, &authority_password))
    return nullptr;
  if (!id || !authority_file || !authority_password)
    return nullptr;

  Py_BEGIN_ALLOW_THREADS;

  try
    {
      auto passport = create_passport(id, authority_file, authority_password);
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
      show();
      PyErr_SetString(metalib_MetaError, err.what());
    }

  Py_END_ALLOW_THREADS;

  return ret;
}
