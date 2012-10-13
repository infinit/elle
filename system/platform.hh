#ifndef ELLE_SYSTEM_PLATFORM_HH
# define ELLE_SYSTEM_PLATEFORM_HH

#include <config.hh>

namespace elle
{
  namespace system
  {

/// Linux-specific
///
/// Note that the _linux_ macro is undefined since generating conflicts.
#if defined(INFINIT_LINUX)
# undef linux
#endif

  }
}

#endif
