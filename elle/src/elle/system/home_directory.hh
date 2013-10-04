#ifndef ELLE_SYSTEM_HOME_DIRECTORY_HH
# define ELLE_SYSTEM_HOME_DIRECTORY_HH

# include <boost/filesystem/path.hpp>

namespace elle
{
  namespace system
  {
    /// Returns the path to the home directory of the current user.
    /// A fallback if the system cannot give that information is to use
    /// environment variables. If that fails too, the last fallback is "/tmp"
    /// on Unices, and "C:\" on Windows.
    boost::filesystem::path
    home_directory();
  }
}

#endif
