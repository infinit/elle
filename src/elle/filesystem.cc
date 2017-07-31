#include <elle/filesystem.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.filesystem");

namespace elle
{
  void
  try_remove(bfs::path const& path)
  {
    boost::system::error_code erc;
    bfs::remove(path, erc);
    if (erc)
      ELLE_WARN("cannot remove file %s: %s", path, erc.message());
  }
}

namespace std
{
  std::size_t
  hash<boost::filesystem::path>::operator()(
    boost::filesystem::path const& p) const
  {
    return hash_value(p);
  }
}
