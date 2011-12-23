//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        Alexandre Bique [thu sep 15 10:58:52 2011]
//

#ifndef ELLE_SYSTEM_PLATFORM_HH
# define ELLE_SYSTEM_PLATEFORM_HH

namespace elle
{
  namespace system
  {

//
// ---------- macros ----------------------------------------------------------
//

///
/// windows-specific
///
# if defined(__WIN32__)
#  define INFINIT_WIN32
# endif

///
/// unix-specific
///
/// note that the _unix_ macro is undefined since generating conflicts.
///
# if defined(__unix__)
#  undef unix
#  define INFINIT_UNIX
# endif

  }
}

#endif
