#ifndef ELLE_SYSTEM_PLATFORM_HH
# define ELLE_SYSTEM_PLATFORM_HH

# include <string>

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

    namespace platform
    {
      std::string
      name();
    }
  }
}

#endif
