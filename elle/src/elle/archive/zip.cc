#include <elle/archive/zip.hh>

namespace elle
{
  namespace archive
  {
    void
    zip(std::vector<boost::filesystem::path> const& files,
        boost::filesystem::path const& path,
        Renamer const& renamer)
    {
      return archive(Format::zip, files, path, renamer);
    }
  }
}
