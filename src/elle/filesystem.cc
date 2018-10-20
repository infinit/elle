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
  }
}
