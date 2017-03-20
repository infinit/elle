#ifndef ELLE_ARCHIVE_TAR_HH
# define ELLE_ARCHIVE_TAR_HH

# include <elle/archive/archive.hh>

namespace elle
{
  namespace archive
  {
    /// Helper for tar archiving.
    ///
    /// @see archive.
    void
    tar(std::vector<boost::filesystem::path> const& files,
        boost::filesystem::path const& path,
        Renamer const& renamer = Renamer());

    /// Helper for tar extraction.
    ///
    /// @see extract.
    void
    untar(boost::filesystem::path const& path);
  }
}

#endif
