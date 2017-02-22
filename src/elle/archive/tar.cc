#include <elle/archive/tar.hh>

namespace elle
{
  namespace archive
  {
    void
    tar(std::vector<boost::filesystem::path> const& files,
        boost::filesystem::path const& path,
        Renamer const& renamer)
    {
      return archive(Format::tar, files, path, renamer);
    }

    void
    untar(boost::filesystem::path const& path)
    {
      return extract(path);
    }
  }
}
