#ifndef ELLE_SYSTEM_USER_PATHS_HH
# define ELLE_SYSTEM_USER_PATHS_HH

# include <boost/filesystem/path.hpp>

namespace elle
{
  namespace system
  {
    /// Returns the path to the home directory of the current user.
    /// A fallback if the system cannot give that information is to use
    /// environment variables. If that fails too, the last fallback is "/tmp"
    /// on Unix-like OSes, and "C:\" on Windows.
    boost::filesystem::path
    home_directory();

    /// Returns the user's download path.
    /// iOS: Documents / Downloads.
    /// OS X: System call for download path.
    /// Other: home_directory() / Downloads
    boost::filesystem::path
    download_directory();
  }
}

#endif
