#pragma once

#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>

#include <elle/compiler.hh>

namespace elle
{
  namespace fs = boost::filesystem;

  /// Make sure the parent directory exists.
  void
  create_parent_directories(fs::path const& p);

  /// Remove file, logging failures but proceeding anyway.
  void
  try_remove(fs::path const& path);
}
