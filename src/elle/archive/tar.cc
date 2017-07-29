#include <elle/archive/tar.hh>

namespace elle
{
  namespace archive
  {
    void
    tar(Paths const& files, bfs::path const& path, Renamer const& renamer)
    {
      return archive(Format::tar, files, path, renamer);
    }

    void
    untar(bfs::path const& path)
    {
      return extract(path);
    }
  }
}
