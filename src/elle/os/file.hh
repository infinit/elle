#ifndef ELLE_OS_FILE_HH
# define ELLE_OS_FILE_HH

# include <string>

# include <boost/filesystem/path.hpp>

///
/// Provide simple methods for file.
///

namespace elle
{
  namespace os
  {
    namespace file
    {
      bool
      exists(std::string const& path);

      bool
      exists(boost::filesystem::path const& path);

      // Give the size of all the files for path.
      uint64_t
      size(std::string const& path);

      uint64_t
      size(boost::filesystem::path const& path);
    }
  }
}

#endif
