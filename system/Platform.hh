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
// ---------- includes --------------------------------------------------------
//

#include <config.hh>

//
// ---------- macros ----------------------------------------------------------
//

///
/// linux-specific
///
/// note that the _linux_ macro is undefined since generating conflicts.
///
#if defined(INFINIT_LINUX)
# undef linux
#endif

  }
}

#endif
