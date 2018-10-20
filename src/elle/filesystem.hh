#pragma once

#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>

#include <elle/compiler.hh>

namespace boost
{
  namespace filesystem
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
  }
}

namespace elle
{
  namespace fs = boost::filesystem;
}
