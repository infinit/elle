#include <elle/filesystem/path.hh>

namespace elle
{
  namespace serialization
  {
    auto
    Serialize<bfs::path>::convert(bfs::path& path)
      -> Type
    {
      // `generic_string` always uses `/` (vs `\`).
      return path.generic_string();
    }

    auto
    Serialize<bfs::path>::convert(Type const& repr)
      -> bfs::path
    {
      return repr;
    }
  }
}
