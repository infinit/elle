#ifndef ELLE_ARCHIVE_ZIP_HH
# define ELLE_ARCHIVE_ZIP_HH

# include <elle/archive/archive.hh>

namespace elle
{
  namespace archive
  {
    void
    zip(std::vector<boost::filesystem::path> const& files,
        boost::filesystem::path const& path,
        Renamer const& renamer = Renamer(),
        bool no_compression = false);

    void
    unzip(boost::filesystem::path const& path);
  }
}

#endif
