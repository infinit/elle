#pragma once

#include <filesystem>

namespace elle
{
  namespace fs = std::filesystem;
}

namespace std::filesystem
{
  /// Make sure the parent directory exists.
  void
  create_parent_directories(path const& p);

  /// Remove file, logging failures but proceeding anyway.
  void
  try_remove(path const& path);

  /// Get the underlying OS handle for a file.
  int
  get_native_handle(std::fstream const& f);

  /// Remove a prefix from a path.
  path
  remove_prefix(path const& prefix, path const& path);
}
