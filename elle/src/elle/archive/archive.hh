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
    };

    typedef
      std::function<boost::filesystem::path(boost::filesystem::path const&)>
      Renamer;
    void
    archive(Format format,
            std::vector<boost::filesystem::path> const& files,
            boost::filesystem::path const& path,
            Renamer const& renamer);
  }
}

#endif
