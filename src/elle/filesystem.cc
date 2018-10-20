#include <elle/filesystem.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.filesystem");

namespace boost
{
  namespace filesystem
  {
    void
    create_parent_directories(path const& p)
    {
      auto const dir = p.parent_path().empty() ? "." : p.parent_path();
      create_directories(dir);
    }

    void
    try_remove(path const& path)
    {
      boost::system::error_code erc;
      remove(path, erc);
      if (erc)
        ELLE_WARN("cannot remove file %s: %s", path, erc.message());
    }
  }
}
