#pragma once

#include <functional>
#include <vector>

#include <boost/optional.hpp>

#include <elle/compiler.hh>
#include <elle/filesystem.hh>

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

    using Renamer =
      std::function<boost::filesystem::path(boost::filesystem::path const&)>;
    /// Returns true to exclude the file.
    using Excluder =
      std::function<bool(boost::filesystem::path const&)>;

    /// Create an archive containing \a list of files.
    ///
    /// @param format The type of archive.
    /// @param files The paths of the files to archive.
    /// @param path Where to write the resulting archive.
    /// @param renamer A function to rename entries.
    /// @param excluder A function to exclude files.
    /// @param ignore_failure Ignore failure (like non-existent files, etc.)
    ELLE_API
    void
    archive(Format format,
            std::vector<boost::filesystem::path> const& files,
            boost::filesystem::path const& path,
            Renamer const& renamer = Renamer(),
            Excluder const& excluder = Excluder(),
            bool ignore_failure = false);

    /// Extract an archive to a given path.
    ///
    /// The extract function supports all formats, no need to specify it
    ///
    /// @param archive The path of the archive.
    /// @param output An optional location where to output the archive. If
    //                unspecified, archive will be extracted in its parent
    ///               folder.
    ELLE_API
    void
    extract(boost::filesystem::path const& archive,
            boost::optional<boost::filesystem::path> const& output = boost::none);
  }
}
