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
    /// @param files          The paths of the files to archive.
    /// @param path           Where to write the resulting archive.
    /// @param renamer        A function to rename entries.
    /// @param no_compression Whether the content of the archive should be
    ///                       compressed or not.
    void
    zip(Paths const& files, fs::path const& path,
        Renamer const& renamer = {},
        bool no_compression = false);

    /// Helper for zip extraction.
    ///
    /// @see extract.
    void
    unzip(fs::path const& path);
  }
}
