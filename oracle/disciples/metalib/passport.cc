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
#include "elle/core/String.hh"

#include "lune/Passport.hh"

// XXX When Qt is out, remove this
#ifdef slots
# undef slots
#endif
#ifdef slot
# undef slot
#undef

#include "metalib.hh"
#include "passport.hh"

//
// ---------- functions  ------------------------------------------------------
//

static lune::Passport create_passport(elle::String const& authority_file,
                                      elle::String const& authority_password)
{
  lune::Authority     authority;
  elle::Path          authority_path;
  lune::Passport      passport;

  if (authority_path.Create(authority_file) == elle::StatusError)
    throw std::runtime_error("unable to create authority path");

  // load the authority file
  if (authority.Load(authority_path) == elle::StatusError)
    throw std::runtime_error("unable to load the authority file");

  // decrypt the authority.
  if (authority.Decrypt(authority_password) == elle::StatusError)
    throw std::runtime_error("unable to decrypt the authority");

  //
  // create the passport.
  //
  {
    hole::Label               label;
    elle::Region              region;

    // generate a random region.
    if (elle::Random::Generate(region) == elle::StatusError)
      throw std::runtime_error("unable to generate a random region");

    // create a label.
    if (label.Create(region) == elle::StatusError)
      throw std::runtime_error("unable to create a label");

    // create the passport.
    if (passport.Create(label) == elle::StatusError)
      throw std::runtime_error("unable to create the passport");

    // seal the passport.
    if (passport.Seal(authority) == elle::StatusError)
      throw std::runtime_error("unable to seal the passport");
  }

  return passport;
}

extern "C" PyObject* metalib_generate_passport(PyObject* self, PyObject* args)
{
  PyObject* ret = nullptr;
  char const* authority_file = nullptr,
            * authority_password = nullptr;
  if (!PyArg_ParseTuple(args, "ss:generate_passport",
                        &authority_file, &authority_password))
    return nullptr;
  if (!authority_file || !authority_password)
    return nullptr;

  Py_BEGIN_ALLOW_THREADS;

  try
    {
      auto passport = create_passport(authority_file, authority_password);
      elle::String passport_string;
      if (passport.Save(passport_string) != elle::StatusError)
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
