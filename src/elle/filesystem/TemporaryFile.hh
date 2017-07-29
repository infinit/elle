#pragma once

#include <elle/attribute.hh>
#include <elle/filesystem.hh>
#include <elle/filesystem/TemporaryDirectory.hh>

namespace elle
{
  namespace filesystem ELLE_API
  {
    /// A specified file name, but in a temporary directory.
    ///
    /// This does not create the file, but it does create the
    /// directory (and removes it on destruction).
    class TemporaryFile
    {
    public:
      TemporaryFile(std::string const& name);

    private:
      ELLE_ATTRIBUTE_R(TemporaryDirectory, directory);
      ELLE_ATTRIBUTE_R(bfs::path, path);
    };
  }
}
