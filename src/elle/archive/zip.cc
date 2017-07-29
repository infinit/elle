#include <elle/archive/zip.hh>

namespace elle
{
  namespace archive
  {
    void
    zip(Paths const& files, bfs::path const& path,
        Renamer const& renamer, bool no_compression)
    {
      return archive(no_compression ? Format::zip_uncompressed : Format::zip,
                     files, path, renamer);
    }
    void
    unzip(bfs::path const& path)
    {
      return extract(path);
    }
  }
}
