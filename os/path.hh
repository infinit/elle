#ifndef  ELLE_OS_PATH_HH
# define ELLE_OS_PATH_HH

# include <string>

///
/// Provides simple manipulators for path.
///

namespace elle
{
  namespace os
  {
    namespace path
    {

      /// Form a path from parts using os separator.
      template<typename... T>
      std::string join(T const&... parts);


      /// Returns true if the path refers to an existing file.
      bool exists(std::string const& path);

    }
  }
}

# include <elle/os/path.hxx>

#endif

