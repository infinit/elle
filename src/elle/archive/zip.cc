#include <elle/archive/zip.hh>

namespace elle
{
  namespace archive
  {
    void
    zip(std::vector<boost::filesystem::path> const& files,
        boost::filesystem::path const& path,
        Renamer const& renamer,
        bool no_compression)
    {
      return archive(no_compression ? Format::zip_uncompressed : Format::zip,
                     files, path, renamer);
    }
    void
    unzip(boost::filesystem::path const& path)
    {
      return extract(path);
    }
  }
}
