#include <elle/filesystem/path.hh>

namespace elle
{
  namespace serialization
  {
    Serialize<bfs::path>::Type
    Serialize<bfs::path>::convert(bfs::path& path)
    {
      return path.generic_string();
    }

    bfs::path
    Serialize<bfs::path>::convert(
      Serialize<bfs::path>::Type const& repr)
    {
      return bfs::path(repr);
    }
  }
}
