#ifndef ELLE_ARCHIVE_ZIP_HH
# define ELLE_ARCHIVE_ZIP_HH

# include <elle/archive/archive.hh>

namespace elle
{
  namespace archive
  {
    /// Helper for zip archiving.
    ///
    /// @see archive.
    ///
    /// @param no_compression Whether the content of the archive should be
    ///                       compressed or not.
    void
    zip(std::vector<boost::filesystem::path> const& files,
        boost::filesystem::path const& path,
        Renamer const& renamer = Renamer(),
        bool no_compression = false);

    /// Helper for zip extraction.
    ///
    /// @see extract.
    void
    unzip(boost::filesystem::path const& path);
  }
}

#endif
