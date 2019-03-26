#include <elle/filesystem.hh>

#include <fstream>

#include <elle/log.hh>
#include <elle/range.hh>

ELLE_LOG_COMPONENT("elle.filesystem");

namespace std::filesystem
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
    error_code erc;
    remove(path, erc);
    if (erc)
      ELLE_WARN("cannot remove file {}: {}", path, erc.message());
  }

  int
  get_native_handle(std::fstream const& f)
  {
    // Hackish, but safe.
    class filebuf_hack
      : public std::filebuf
    {
    public:
      int handle() { return _M_file.fd(); }
    };
    return static_cast<filebuf_hack&>(*f.rdbuf()).handle();
  }
  path
  remove_prefix(path const& prefix, path const& p)
  {
    auto it = p.begin();
    for (auto const& comp: prefix)
      if (*it++ != comp)
        elle::err("{} is not a prefix of {}", prefix, p);
    // No way that I know of to properly build a path directly from a
    // pair of iterators.
    path res;
    for (auto const& comp: elle::as_range(it, p.end()))
      res /= comp;
    return res;
  }
}
