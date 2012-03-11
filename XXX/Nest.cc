//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [sat dec 24 14:08:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Nest.hh>

using namespace nucleus::proton;

//
// ---------- definitions -----------------------------------------------------
//

///
/// XXX
///
elle::Callback<
  elle::Status,
  elle::Parameters<
    Handle&
    >
  >                                     Nest::Load;

///
/// XXX
///
elle::Callback<
  elle::Status,
  elle::Parameters<
    Handle&
    >
  >                                     Nest::Unload;

//
// ---------- methods ---------------------------------------------------------
//

///
/// XXX
///
elle::Status            Nest::Setup(
  const elle::Callback<
    elle::Status,
    elle::Parameters<
      Handle&
      >
    >&                                                          load,
  const elle::Callback<
    elle::Status,
    elle::Parameters<
      Handle&
      >
    >&                                                          unload)
{
  // set the callbacks.
  Nest::Load = load;
  Nest::Unload = unload;

  return elle::StatusOk;
}
