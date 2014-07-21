#ifndef ELLE_ARCHIVE_HH
# define ELLE_ARCHIVE_HH

# include <functional>
# include <vector>

# include <elle/filesystem.hh>

namespace elle
{
  namespace archive
  {
    enum class Format
    {
      zip,
      tar,
      tar_gzip,
      tar_bzip2,
      zip_uncompressed
    };

    typedef
      std::function<boost::filesystem::path(boost::filesystem::path const&)>
      Renamer;
    typedef
      std::function<bool(boost::filesystem::path const&)>
      Excluder; /// Returns true to exclude the file

    void
    archive(Format format,
            std::vector<boost::filesystem::path> const& files,
            boost::filesystem::path const& path,
            Renamer const& renamer = Renamer(),
            Excluder const& excluder = Excluder());
  }
}

#endif
