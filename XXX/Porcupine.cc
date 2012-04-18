//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sat oct  8 22:42:48 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Porcupine.hh>

using namespace nucleus::proton;

//
// ---------- definitions -----------------------------------------------------
//

///
/// XXX in bytes
///
const elle::Natural32                   Porcupine<>::Default::Length = 16;

///
/// XXX
///
elle::cryptography::SecretKey                         Porcupine<>::Default::Secret;

//
// ---------- definitions -----------------------------------------------------
//

///
/// XXX
///
elle::concurrency::Callback<
  elle::Status,
  elle::radix::Parameters<
    Block*,
    Handle&
    >
  >                                     Porcupine<>::Attach;

///
/// XXX
///
elle::concurrency::Callback<
  elle::Status,
  elle::radix::Parameters<
    Handle&
    >
  >                                     Porcupine<>::Detach;

///
/// XXX
///
elle::concurrency::Callback<
  elle::Status,
  elle::radix::Parameters<
    Handle&
    >
  >                                     Porcupine<>::Load;

///
/// XXX
///
elle::concurrency::Callback<
  elle::Status,
  elle::radix::Parameters<
    Handle&
    >
  >                                     Porcupine<>::Unload;

//
// ---------- static methods --------------------------------------------------
//

///
/// this method initializes the porcupine.
///
elle::Status        Porcupine<>::Initialize(
  const elle::concurrency::Callback<
  elle::Status,
  elle::radix::Parameters<
    Block*,
    Handle&
    >
  >&                                                        attach,
  const elle::concurrency::Callback<
  elle::Status,
  elle::radix::Parameters<
    Handle&
    >
  >&                                                        detach,
  const elle::concurrency::Callback<
  elle::Status,
  elle::radix::Parameters<
    Handle&
    >
  >&                                                        load,
  const elle::concurrency::Callback<
  elle::Status,
  elle::radix::Parameters<
    Handle&
    >
  >&                                                        unload)
{
  // set the callbacks.
  Porcupine<>::Attach = attach;
  Porcupine<>::Detach = detach;
  Porcupine<>::Load = load;
  Porcupine<>::Unload = unload;

  // initialize the default secret key.
  //
  // this is required for nodules' footprint to be computed properly.
  if (Porcupine<>::Default::Secret.Create(
        elle::String(Porcupine<>::Default::Length, 'B')) == elle::Status::Error)
    escape("unable to generate the default key");

  //
  // catalog-specific initialization.
  //
  {
    // initialize the seam.
    if (Seam<nucleus::Catalog>::Initialize() == elle::Status::Error)
      escape("unable to initialize the seam");

    // initialize the quill.
    if (Quill<nucleus::Catalog>::Initialize() == elle::Status::Error)
      escape("unable to initialize the quill");
  }

  // XXX

  return elle::Status::Ok;
}

///
/// this method cleans the porcupine.
///
elle::Status        Porcupine<>::Clean()
{
  //
  // catalog-specific cleaning.
  //
  {
    // clean the quill.
    if (Quill<nucleus::Catalog>::Clean() == elle::Status::Error)
      escape("unable to clean the quill");

    // clean the seam.
    if (Seam<nucleus::Catalog>::Clean() == elle::Status::Error)
      escape("unable to clean the seam");
  }

  // XXX

  return elle::Status::Ok;
}
