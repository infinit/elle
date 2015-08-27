#include <reactor/filesystem.hh>

#include <unordered_map>

#include <reactor/fuse.hh>
#include <reactor/thread.hh>
#include <reactor/scheduler.hh>

#include <boost/filesystem.hpp>

#include <elle/os/environ.hh>

#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION 26

#include <fuse/fuse.h>

ELLE_LOG_COMPONENT("reactor.filesystem.fuse");

namespace std
{
  template<> struct hash<boost::filesystem::path>
  {
    size_t operator()(const boost::filesystem::path& p) const
    {
      return boost::filesystem::hash_value(p);
    }
  };
}

namespace reactor
{
  namespace filesystem
  {
    typedef std::shared_ptr<Path> PathPtr;

    static int fusop_getattr(const char *path, struct stat *stbuf)
    {
      ELLE_DEBUG_SCOPE("fusop_getattr %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->stat(stbuf);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error stating %s: %s", path, e.what());
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
       off_t offset, struct fuse_file_info *fi)
    {
      ELLE_DEBUG_SCOPE("fusop_readdir %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->list_directory(
          [&](std::string const& filename, struct stat* stbuf) {
            filler(buf, filename.c_str(), stbuf, 0);
        });
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error reading dir %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_open(const char *path, struct fuse_file_info *fi)
    {
      ELLE_DEBUG_SCOPE("fusop_open %s %s", path, fi->flags);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        auto handle = p->open(fi->flags, 0);
        fi->fh = (decltype(fi->fh)) handle.release();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error opening %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_create(const char* path, mode_t mode, fuse_file_info* fi)
    {
      ELLE_DEBUG_SCOPE("fusop_create %s %s %s", path, mode, fi->flags);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        auto handle = p->create(fi->flags, mode);
        fi->fh = (decltype(fi->fh)) handle.release();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error creating %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_unlink(const char* path)
    {
      ELLE_DEBUG_SCOPE("fusop_unlink %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->unlink();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error unlinking %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_mkdir(const char* path, mode_t mode)
    {
      ELLE_DEBUG_SCOPE("fusop_mkdir %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->mkdir(mode);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error mkdiring %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_rmdir(const char* path)
    {
      ELLE_DEBUG_SCOPE("fusop_rmdir %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->rmdir();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error rmdiring %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_rename(const char* path, const char* to)
    {
      ELLE_DEBUG_SCOPE("fusop_rename %s %s", path, to);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->rename(to);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error renaming %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_read(const char *path, char *buf, size_t size, off_t offset,
               struct fuse_file_info *fi)
    {
      ELLE_DEBUG_SCOPE("fusop_read %s sz=%s, offset=%s", path, size, offset);
      try
      {
        Handle* handle = (Handle*)fi->fh;
        return handle->read(elle::WeakBuffer(buf, size), size, offset);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error reading %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_write(const char *path, const char *buf, size_t size, off_t offset,
                struct fuse_file_info *fi)
    {
      ELLE_DEBUG_SCOPE("fusop_write %s sz=%s, offset=%s ", path, size, offset);
      try
      {
        Handle* handle = (Handle*)fi->fh;
        return handle->write(elle::WeakBuffer((void*)buf, size), size, offset);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error writing %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_release(const char *path, struct fuse_file_info *fi)
    {
      ELLE_DEBUG_SCOPE("fusop_release %s", path);
      try
      {
        Handle* handle = (Handle*)fi->fh;
        handle->close();
        delete handle;
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error releasing %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_ftruncate(const char* path, off_t offset,
                    struct fuse_file_info* fi)
    {
      ELLE_DEBUG_SCOPE("fusop_ftruncate %s %s", path, offset);
      try
      {
        Handle* handle = (Handle*)fi->fh;
        handle->ftruncate(offset);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error truncating %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_readlink(const char* path, char* buf, size_t len)
    {
      ELLE_DEBUG_SCOPE("fusop_readlink %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        auto target = p->readlink();
        auto starget = target.string();
        strncpy(buf, starget.c_str(), len);
        return 0;
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on readlink %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_symlink(const char* target, const char* where)
    {
      ELLE_DEBUG_SCOPE("fusop_symlink %s %s", target, where);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(where);
        p->symlink(target);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on symlink %s: %s", where, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_link(const char* path, const char* to)
    {
      ELLE_DEBUG_SCOPE("fusop_link %s %s", path, to);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->link(to);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on link %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_chmod(const char* path, mode_t mode)
    {
      ELLE_DEBUG_SCOPE("fusop_chmod %s %s", path, mode);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->chmod(mode);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on chmod %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_chown(const char* path, uid_t uid, gid_t gid)
    {
      ELLE_DEBUG_SCOPE("fusop_chown %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->chown(uid, gid);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on chown %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_statfs(const char* path, struct ::statvfs* svfs)
    {
      ELLE_DEBUG_SCOPE("fusop_statfs %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->statfs(svfs);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on statfs %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_utimens(const char* path, const struct timespec tv[2])
    {
      ELLE_DEBUG_SCOPE("fusop_utimens %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->utimens(tv);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on readlink %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_truncate(const char* path, off_t new_size)
    {
      ELLE_DEBUG_SCOPE("fusop_truncate %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->truncate(new_size);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on truncate %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static int fusop_setxattr(const char *path, const char *key,
                               const char *val, size_t valsize, int flags
#ifdef INFINIT_MACOSX
                              , uint32_t position
#endif
                              )
    {
            ELLE_DEBUG_SCOPE("fusop_setxattr %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->setxattr(key, std::string(val, valsize), flags);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on setxattr %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_getxattr(const char *path, const char *key,
                              char *val, size_t valsize
#ifdef INFINIT_MACOSX
                              , uint32_t position
#endif
                             )
    {
      ELLE_DEBUG_SCOPE("fusop_getxattr %s buf %s", path, valsize);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        std::string res = p->getxattr(key);
        if (val)
          memcpy(val, res.c_str(), std::min(res.size()+1, valsize));
        return res.size();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on getxattr %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_listxattr(const char* path, char* buf, size_t size)
    {
      ELLE_DEBUG_SCOPE("fusop_listxattr %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        std::vector<std::string> res = p->listxattr();
        std::string packed;
        for (auto const& s: res)
        {
          packed += s + (char)0;
        }
        if (buf)
          memcpy(buf, packed.data(), std::min(size, packed.size()));
        return packed.size();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on listxattr %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_removexattr(const char *path, const char *key)
    {
      ELLE_DEBUG_SCOPE("fusop_removexattr %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->removexattr(key);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on removexattr %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    class FileSystemImpl
    {
    public:
      std::shared_ptr<Path> fetch_recurse(boost::filesystem::path path);
      std::unique_ptr<Operations> _operations;
      bool _full_tree;
      FuseContext _fuse;
      std::string _where;
      std::unordered_map<boost::filesystem::path, std::shared_ptr<Path>> _cache;
    };

    FileSystem::FileSystem(std::unique_ptr<Operations> op, bool full_tree)
      : _impl(new FileSystemImpl{std::move(op), full_tree})
    {
      this->_impl->_operations->filesystem(this);
    }

    FileSystem::~FileSystem()
    {
      unmount();
      delete _impl;
    }

    void
    FileSystem::mount(boost::filesystem::path const& where,
                      std::vector<std::string> const& options)
    {
      _impl->_where = where.string();
      fuse_operations ops;
      memset(&ops, 0, sizeof(ops));
      ops.getattr = fusop_getattr;
      ops.readdir = fusop_readdir;
      ops.open = fusop_open;
      ops.read = fusop_read;
      ops.write = fusop_write;
      ops.release = fusop_release;
      ops.create = fusop_create;
      ops.unlink = fusop_unlink;
      ops.mkdir = fusop_mkdir;
      ops.rmdir = fusop_rmdir;
      ops.rename = fusop_rename;
      ops.readlink = fusop_readlink;
      ops.symlink = fusop_symlink;
      ops.link = fusop_link;
      ops.chmod = fusop_chmod;
      ops.chown = fusop_chown;
      ops.statfs = fusop_statfs;
      ops.utimens = fusop_utimens;
      ops.truncate = fusop_truncate;
      ops.ftruncate = fusop_ftruncate;
      ops.setxattr = fusop_setxattr;
      ops.getxattr = fusop_getxattr;
      ops.listxattr = fusop_listxattr;
      ops.removexattr = fusop_removexattr;
      _impl->_fuse.create(where.string(), options, &ops, sizeof(ops), this);
      _impl->_fuse.on_loop_exited([this] { this->unmount();});
      if (!elle::os::getenv("INFINIT_FUSE_POOL", "").empty())
      {
        int nt = 5;
        std::string nthread = elle::os::getenv("INFINIT_FUSE_POOL", "");
        if (!nthread.empty())
          nt = std::stoi(nthread);
        ELLE_TRACE("Pool mode with %s workers", nt);
        _impl->_fuse.loop_pool(nt);
      }
      else if (!elle::os::getenv("INFINIT_FUSE_THREAD", "").empty())
      {
        ELLE_TRACE("Thread mode");
        _impl->_fuse.loop_mt();
      }
      else
      {
        ELLE_TRACE("Single mode");
        _impl->_fuse.loop();
      }
    }

    void
    FileSystem::unmount()
    {
      ELLE_TRACE("unmount");
      if (!_impl->_where.empty())
      {
        _impl->_fuse.destroy();
        _impl->_where = "";
        this->_signal();
      }
    }

    bool
    FileSystem::_wait(Thread* thread)
    {
      if (!this->_impl)
        return false;
      else
        return Waitable::_wait(thread);
    }

    std::shared_ptr<Path>
    FileSystemImpl::fetch_recurse(boost::filesystem::path path)
    {
      if (path == "" || path == "\\")
        path = "/";
      ELLE_DEBUG("fetch_recurse on %s", path);
      auto it = _cache.find(path);
      if (it != _cache.end())
      {
        ELLE_DEBUG("cache hit: %x", it->second.get());
        return it->second;
      }
      else
      {
        ELLE_DEBUG("cache miss");
        if (path == "/")
        {
          auto p = _operations->path("/");
          if (p->allow_cache())
            _cache[path] = p;
          return p;
        }
        std::shared_ptr<Path> parent = fetch_recurse(path.parent_path());
        auto p = parent->child(path.filename().string());
        if (p->allow_cache())
          _cache[path] = p;
        return p;
      }
    }

    std::shared_ptr<Path>
    FileSystem::path(std::string const& opath)
    {
      std::string spath(opath);
      if (spath == "" || spath == "\\")
        spath = "/";
      ELLE_ASSERT(_impl);
      if (_impl->_full_tree)
      {
        return _impl->fetch_recurse(spath);
      }
      else
      {
        auto it = _impl->_cache.find(spath);
        if (it == _impl->_cache.end())
        {
          auto res = _impl->_operations->path(spath);
          if (res->allow_cache())
            _impl->_cache[spath] = res;
          return res;
        }
        else
          return it->second;
      }
    }

    std::shared_ptr<Path>
    FileSystem::extract(std::string const& path)
    {
      auto it = _impl->_cache.find(path);
      if (it == _impl->_cache.end())
        return {};
      auto res = it->second;
      _impl->_cache.erase(path);
      return res;
    }

    std::shared_ptr<Path>
    FileSystem::set(std::string const& path,
                    std::shared_ptr<Path> new_content)
    {
      std::shared_ptr<Path> res = extract(path);
      _impl->_cache[path] = new_content;
      return res;
    }

    std::shared_ptr<Path>
    FileSystem::get(std::string const& path)
    {
      auto it = _impl->_cache.find(path);
      if (it == _impl->_cache.end())
        return nullptr;
      else
        return it->second;
    }
  }
}
