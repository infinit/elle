#pragma once

#include <elle/archive/archive.hh>

namespace elle
{
  namespace archive
  {
    /// Helper for tar archiving.
    ///
    /// @see archive.
    void
    tar(std::vector<bfs::path> const& files,
        bfs::path const& path,
        Renamer const& renamer = Renamer());

    /// Helper for tar extraction.
    ///
    /// @see extract.
    void
    untar(bfs::path const& path);
  }
}
