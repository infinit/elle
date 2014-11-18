#include <reactor/filesystem.hh>

#include <unordered_map>

#include <reactor/fuse.hh>
#include <reactor/thread.hh>
#include <reactor/scheduler.hh>

#include <boost/filesystem.hpp>

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
    static int fusop_getattr(const char *path, struct stat *stbuf)
    {
      ELLE_DEBUG("fusop_getattr %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.stat(stbuf);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error stating %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
       off_t offset, struct fuse_file_info *fi)
    {
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.list_directory(
          [&](std::string const& filename, struct stat* stbuf) {
            filler(buf, filename.c_str(), stbuf, 0);
        });
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error stating %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static int fusop_open(const char *path, struct fuse_file_info *fi)
    {
      ELLE_DEBUG("fusop_open %s %s", path, fi->flags);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        auto handle = p.open(fi->flags, 0);
        fi->fh = (decltype(fi->fh)) handle.release();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error stating %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static int fusop_create(const char* path, mode_t mode, fuse_file_info* fi)
    {
      ELLE_DEBUG("fusop_create %s %s %s", path, mode, fi->flags);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        auto handle = p.create(fi->flags, mode);
        fi->fh = (decltype(fi->fh)) handle.release();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error stating %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static int fusop_unlink(const char* path)
    {
      ELLE_DEBUG("fusop_unlink %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.unlink();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error unlinking %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static int fusop_mkdir(const char* path, mode_t mode)
    {
      ELLE_DEBUG("fusop_mkdir %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.mkdir(mode);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error mkdiring %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static int fusop_rmdir(const char* path)
    {
      ELLE_DEBUG("fusop_rmdir %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.rmdir();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error rmdiring %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static int fusop_rename(const char* path, const char* to)
    {
      ELLE_DEBUG("fusop_rename %s %s", path, to);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.rename(to);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error renaming %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static int fusop_read(const char *path, char *buf, size_t size, off_t offset,
                          struct fuse_file_info *fi)
    {
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

    static int fusop_write(const char *path, const char *buf, size_t size, off_t offset,
                           struct fuse_file_info *fi)
    {
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

    static int fusop_release(const char *path, struct fuse_file_info *fi)
    {
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

    static int fusop_readlink(const char* path, char* buf, size_t len)
    {
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        auto target = p.readlink();
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

    static int fusop_symlink(const char* path, const char* to)
    {
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.symlink(to);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on symlink %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }
    static int fusop_link(const char* path, const char* to)
    {
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.link(to);
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
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.chmod(mode);
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
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.chown(uid, gid);
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
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.statfs(svfs);
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
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.utimens(tv);
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
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        Path& p = fs->path(path);
        p.truncate(new_size);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on truncate %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    class FileSystemImpl
    {
    public:
      Path& fetch_recurse(boost::filesystem::path const& path);
      std::unique_ptr<Operations> _operations;
      bool _full_tree;
      FuseContext _fuse;
      std::string _where;
      std::unordered_map<boost::filesystem::path, std::unique_ptr<Path>> _cache;
    };

    FileSystem::FileSystem(std::unique_ptr<Operations> op, bool full_tree)
    : _impl(new FileSystemImpl{std::move(op), full_tree})
    {
    }
    FileSystem::~FileSystem()
    {
      unmount();
      delete _impl;
    }
    void FileSystem::mount(boost::filesystem::path const& where,
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
      _impl->_fuse.create(where.string(), options, &ops, sizeof(ops), this);
      _impl->_fuse.loop_mt();
    }
    void FileSystem::unmount()
    {
      if (!_impl->_where.empty())
        _impl->_fuse.destroy();
      _impl->_where = "";
    }
    Path& FileSystemImpl::fetch_recurse(boost::filesystem::path const& path)
    {
      ELLE_DEBUG("fetch_recurse on %s", path);
      auto it = _cache.find(path);
      if (it != _cache.end())
      {
        ELLE_DEBUG("cache hit");
        return *it->second;
      }
      else
      {
        ELLE_DEBUG("cache miss");
        if (path == "/")
        {
          auto p = _operations->path("/");
          auto ptr = p.get();
          ELLE_ASSERT(ptr);
          _cache[path] = std::move(p);
          return *ptr;
        }
        Path& parent = fetch_recurse(path.parent_path());
        auto p = parent.child(path.filename().string());
        auto ptr = p.get();
        ELLE_ASSERT(ptr);
        _cache[path] = std::move(p);
        return *ptr;
      }
    }
    Path& FileSystem::path(std::string const& spath)
    {
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
          auto ptr = res.get();
          _impl->_cache[spath] = std::move(res);
          return *ptr;
        }
        else
          return *it->second;
      }
    }
  }
}