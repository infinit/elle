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
#define NestRegister(_handle_)						\
  if (Nest::Register(_handle._object) == elle::StatusError)		\
    escape("unable to register the new block to the nest");

///
/// this macro-function makes it easier to load a block.
///
#define NestLoad(_handle_)						\
  if (_handle_.address != Address::Null)				\
    {									\
      if (_handle_._object == NULL)					\
	{								\
	  printf("NEST LOAD\n");					\
	}								\
    }									\
  if (_handle_._object != NULL)

///
/// this macro-function makes it easier to unload a block.
///
#define NestUnload(_handle_)						\
  printf("NEST UNLOAD\n");

  }
}

#endif
