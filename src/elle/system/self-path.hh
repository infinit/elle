#ifndef ELLE_SELF_PATH_HH
# define ELLE_SELF_PATH_HH

# include <boost/filesystem.hpp>

namespace elle
{
  namespace system
  {
    /// Path to the current executable.
    ///
    /// @returns path to the current executable.
    /// @throw elle::Error if unable to determine current executable path.
    boost::filesystem::path
    self_path();
  }
}

#endif
