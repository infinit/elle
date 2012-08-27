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
      std::string
      join(T const&... parts);


      /// Returns true if the path refers to an existing file.
      bool
      exists(std::string const& path);

      /// Returns the absolute version of path. If canonical is true, it
      /// removes all symbolic link, '.' and '..' occurences.
      std::string
      absolute(std::string const& path,
               bool canonical = false);

      /// Check whether or not a path point to a directory.
      bool
      is_directory(std::string const& path);

      /// Create a new directory. An already existing directory is not an error.
      void
      make_directory(std::string const& path);

      /// Create all required path components directories.
      void
      make_path(std::string const& path);

      /// Makes a symbolic link to oldname named newname.
      void
      make_symlink(std::string const& oldname,
                   std::string const& newname);
    }
  }
}

# include <elle/os/path.hxx>

#endif

