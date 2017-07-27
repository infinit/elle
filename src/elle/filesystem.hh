#pragma once

#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>

#include <elle/compiler.hh>

namespace elle
{
  namespace bfs = boost::filesystem;

  /// Remove file, logging failures but proceeding anyway.
  void
  try_remove(bfs::path const& path);
}
