#pragma once

#include <elle/archive/archive.hh>

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
    zip(std::vector<bfs::path> const& files,
        bfs::path const& path,
        Renamer const& renamer = Renamer(),
        bool no_compression = false);

    /// Helper for zip extraction.
    ///
    /// @see extract.
    void
    unzip(bfs::path const& path);
  }
}
