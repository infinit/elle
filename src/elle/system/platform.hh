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
      /// The OS running (Linux, MacOSX, Windows, iOS).
      std::string
      os_name();

      /// The version of the OS.
      std::string
      os_version();

      /// Helper that concatenates the OS and version.
      std::string
      os_description();
    }
  }
}

#endif
