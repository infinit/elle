#ifndef ELLE_ARCHIVE_ZIP_HH
# define ELLE_ARCHIVE_ZIP_HH

# include <boost/filesystem.hpp>

namespace elle
{
  namespace archive
  {
    void
    zip(std::vector<boost::filesystem::path> const& files,
        boost::filesystem::path const& path);
  }
}

#endif
