#pragma once

#include <string>

#include <elle/filesystem/path.hh>

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
