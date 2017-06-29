#pragma once

#include <string>

#include <elle/attribute.hh>
#include <elle/compiler.hh>
#include <elle/filesystem.hh>

namespace elle
{
  namespace filesystem
  {
    /// Temporary directory, removed on destruction.
    class ELLE_API TemporaryDirectory
    {
    public:
      /// Create a temporary named after the executable.
      TemporaryDirectory();
      TemporaryDirectory(std::string const& name);
      ~TemporaryDirectory();

    private:
      ELLE_ATTRIBUTE_R(bfs::path, path);
      ELLE_ATTRIBUTE(bfs::path, root);
    };
  }
}
