#pragma once

#include <elle/attribute.hh>
#include <elle/filesystem.hh>
#include <elle/filesystem/TemporaryDirectory.hh>

namespace elle
{
  namespace filesystem
  {
    class ELLE_API TemporaryFile
    {
    public:
      TemporaryFile(std::string const& name);
    private:
      ELLE_ATTRIBUTE_R(TemporaryDirectory, directory);
      ELLE_ATTRIBUTE_R(bfs::path, path);
    };
  }
}
