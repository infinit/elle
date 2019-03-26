#pragma once

#include <functional>
#include <vector>

#include <boost/optional.hpp>

#include <elle/compiler.hh>
#include <elle/filesystem/path.hh>

namespace elle
{
  namespace archive ELLE_API
  {
    namespace fs = std::filesystem;

    enum class Format
    {
      zip,
      tar,
      tar_gzip,
      tar_bzip2,
      zip_uncompressed
    };

    /// Set of files.
    using Paths = std::vector<fs::path>;
    /// Map source names to names in the archive.
    using Renamer = std::function<auto (fs::path const&) -> fs::path>;
    /// Return true to exclude the file.
    using Excluder = std::function<auto (fs::path const&) -> bool>;

    /// Create an archive containing @a list of files.
    ///
    /// @param format         The type of archive.
    /// @param files          The paths of the files to archive.
    /// @param path           Where to write the resulting archive.
    /// @param renamer        A function to rename entries.
    /// @param excluder       A function to exclude files.
    /// @param ignore_failure Ignore failure (like non-existent files, etc.)
    void
    archive(Format format,
            Paths const& files,
            fs::path const& path,
            Renamer const& renamer = {},
            Excluder const& excluder = {},
            bool ignore_failure = false);

    /// Extract an archive to a given path.
    ///
    /// The extract function supports all formats, no need to specify it.
    ///
    /// @param archive The path of the archive.
    /// @param output An optional location where to output the archive. If
    //                unspecified, archive will be extracted in its parent
    ///               folder.
    void
    extract(fs::path const& archive,
            boost::optional<fs::path> const& output = {});
  }
}
