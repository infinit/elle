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
    tar(Paths const& files, bfs::path const& path,
        Renamer const& renamer = {});

    /// Helper for tar extraction.
    ///
    /// @see extract.
    void
    untar(bfs::path const& path);
  }
}
