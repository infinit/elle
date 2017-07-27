#include <elle/filesystem.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.filesystem");

namespace elle
{
  void
  create_parent_directories(fs::path const& p)
  {
    auto const dir = p.parent_path().empty() ? "." : p.parent_path();
    create_directories(dir);
  }

  void
  try_remove(fs::path const& path)
  {
    boost::system::error_code erc;
    fs::remove(path, erc);
    if (erc)
      ELLE_WARN("cannot remove file %s: %s", path, erc.message());
  }
}
