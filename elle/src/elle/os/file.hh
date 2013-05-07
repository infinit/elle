#ifndef ELLE_OS_FILE_HH
# define ELLE_OS_FILE_HH

# include <string>

///
/// Provide simple methods for file.
///

namespace elle
{
  namespace os
  {
    namespace file
    {
      // Give the size of all the files for path.
      size_t
      size(std::string const& path);
    }
  }
}

#endif
