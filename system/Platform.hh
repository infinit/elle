//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri jan 13 19:16:29 2012]
//

#ifndef ELLE_SYSTEM_PLATFORM_HH
#define ELLE_SYSTEM_PLATEFORM_HH

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
# if defined(_WIN32) || defined(_WIN64)
#  define INFINIT_WINDOWS

///
/// unix-specific
///
/// note that the _unix_ macro is undefined since generating conflicts.
///
# elif defined(__linux) || defined(__unix)
#  undef unix
#  define INFINIT_UNIX

///
/// macos-specific
///
# elif defined(__APPLE__) && defined(__MACH__)
#  define INFINIT_MACOSX

///
/// unknown platform
///
# else
# error "unknown platform"
# endif

  }
}

#endif
