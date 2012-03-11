//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [sat dec 24 13:43:19 2011]
//

#ifndef NUCLEUS_PROTON_NEST_HXX
#define NUCLEUS_PROTON_NEST_HXX

namespace nucleus
{
  namespace proton
  {

//
// ---------- macro-functions -------------------------------------------------
//

///
/// XXX
///
#define NestRegister(_handle_)                                          

    //  if (Nest::Register(_handle._object) == elle::StatusError)       
    // escape("unable to register the new block to the nest");

///
/// this macro-function makes it easier to load a block.
///
/// XXX do not use this macro-function in an implicit scope i.e if, for etc.
///
#define NestLoad(_handle_)                                              \
  if ((_handle_).address != Address::Null)                              \
    {                                                                   \
      if (Nest::Load.Call((_handle_)) == elle::StatusError)             \
        escape("unable to load the handle's block");                    \
    }

///
/// this macro-function makes it easier to unload a block.
///
#define NestUnload(_handle_)                                            \
  if (Nest::Unload.Call((_handle_)) == elle::StatusError)               \
    escape("unable to unload the handle's block");

  }
}

#endif
