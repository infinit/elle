#define _FILE_OFFSET_BITS 64 // This goes before sys/types.h.
#define FUSE_USE_VERSION 26

#include <errno.h>
#include <sys/types.h>

#ifdef INFINIT_LINUX
# include <attr/xattr.h>
#else
# include <sys/xattr.h>
#endif

#include <unordered_map>

#include <fuse/fuse.h>

#include <boost/filesystem.hpp>

#include <elle/os/environ.hh>

#include <reactor/filesystem.hh>
#include <reactor/fuse.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

ELLE_LOG_COMPONENT("reactor.filesystem.fuse");


namespace reactor
{
  namespace filesystem
  {
    typedef std::shared_ptr<Path> PathPtr;

    static
    int
    fusop_getattr(const char* path, struct stat* stbuf)
    {
      ELLE_TRACE_SCOPE("fusop_getattr %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->stat(stbuf);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("filesystem error statting %s: %s", path, e.what());
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_readdir(const char* path,
                  void* buf,
                  fuse_fill_dir_t filler,
                  off_t offset,
                  struct fuse_file_info* fi)
    {
      ELLE_TRACE_SCOPE("fusop_readdir %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->list_directory(
          [&](std::string const& filename, struct stat* stbuf)
          {
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
    fusop_open(const char* path, struct fuse_file_info* fi)
    {
      ELLE_TRACE_SCOPE("fusop_open %s %s", path, fi->flags);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        auto handle = p->open(fi->flags, 0);
        fi->fh = (decltype(fi->fh)) handle.release();
        ELLE_TRACE("handle: %s", fi->fh);
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
      ELLE_TRACE_SCOPE("fusop_create %s %s %s", path, mode, fi->flags);
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
      ELLE_TRACE_SCOPE("fusop_unlink %s", path);
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
      ELLE_TRACE_SCOPE("fusop_mkdir %s", path);
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
      ELLE_TRACE_SCOPE("fusop_rmdir %s", path);
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
      ELLE_TRACE_SCOPE("fusop_rename %s %s", path, to);
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
    fusop_read(const char* path,
               char* buf,
               size_t size,
               off_t offset,
               struct fuse_file_info* fi)
    {
      ELLE_TRACE_SCOPE("fusop_read %s sz=%s, offset=%s", path, size, offset);
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
    fusop_write(const char* path,
                const char* buf,
                size_t size,
                off_t offset,
                struct fuse_file_info* fi)
    {
      ELLE_TRACE_SCOPE("fusop_write %s(%s) sz=%s, offset=%s ",
                       path, fi->fh, size, offset);
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
    fusop_release(const char* path, struct fuse_file_info* fi)
    {
      ELLE_TRACE_SCOPE("fusop_release %s", path);
      try
      {
        std::unique_ptr<Handle> handle((Handle*)fi->fh);
        if (handle)
          handle->close();
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
    fusop_flush(const char* path, struct fuse_file_info* fi)
    {
      ELLE_TRACE_SCOPE("fusop_flush %s(%s)", path, fi->fh);
      try
      {
        Handle* handle = (Handle*)fi->fh;
        if (handle)
          handle->close();
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error flushing %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_ftruncate(const char* path, off_t offset, struct fuse_file_info* fi)
    {
      ELLE_TRACE_SCOPE("fusop_ftruncate %s %s", path, offset);
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
      ELLE_TRACE_SCOPE("fusop_readlink %s", path);
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
      ELLE_TRACE_SCOPE("fusop_symlink %s %s", target, where);
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

    static
    int
    fusop_link(const char* path, const char* to)
    {
      ELLE_TRACE_SCOPE("fusop_link %s %s", path, to);
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

    static
    int
    fusop_chmod(const char* path, mode_t mode)
    {
      ELLE_TRACE_SCOPE("fusop_chmod %s %s", path, mode);
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

    static
    int
    fusop_chown(const char* path, uid_t uid, gid_t gid)
    {
      ELLE_TRACE_SCOPE("fusop_chown %s", path);
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

    static
    int
    fusop_statfs(const char* path, struct ::statvfs* svfs)
    {
      ELLE_TRACE_SCOPE("fusop_statfs %s", path);
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

    static
    int
    fusop_utimens(const char* path, const struct timespec tv[2])
    {
      ELLE_TRACE_SCOPE("fusop_utimens %s", path);
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

    static
    int
    fusop_truncate(const char* path, off_t new_size)
    {
      ELLE_TRACE_SCOPE("fusop_truncate %s", path);
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

    static
    int
    fusop_setxattr(const char* path,
                   const char* key,
                   const char* val,
                   size_t valsize,
                   int flags
#ifdef INFINIT_MACOSX
                   , uint32_t position
#endif
      )
    {
      ELLE_TRACE_SCOPE("fusop_setxattr %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->setxattr(key, std::string(val, valsize), flags);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("error: %s", e.what());
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_getxattr(const char* path,
                   const char* key,
                   char* val,
                   size_t valsize
#ifdef INFINIT_MACOSX
                   , uint32_t position
#endif
      )
    {
      ELLE_TRACE_SCOPE("fusop_getxattr %s buf %s", path, valsize);
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
        if (e.error_code() != ENOATTR)
          ELLE_TRACE("error: %s", e.what());
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_listxattr(const char* path, char* buf, size_t size)
    {
      ELLE_TRACE_SCOPE("fusop_listxattr %s", path);
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

    static
    int
    fusop_removexattr(const char* path, const char* key)
    {
      ELLE_TRACE_SCOPE("fusop_removexattr %s", path);
      try
      {
        FileSystem* fs = (FileSystem*)fuse_get_context()->private_data;
        PathPtr p = fs->path(path);
        p->removexattr(key);
      }
      catch (Error const& e)
      {
        if (e.error_code() != ENOATTR)
          ELLE_TRACE("error: %s", e.what());
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_fsync(const char* path, int datasync, fuse_file_info* fi)
    {
      ELLE_TRACE_SCOPE("fusop_fsync %s %s", path, datasync);
      try
      {
        Handle* handle = (Handle*)fi->fh;
        handle->fsync(datasync);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on fsync %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    static
    int
    fusop_fsyncdir(const char* path, int datasync, fuse_file_info* fi)
    {
      ELLE_TRACE_SCOPE("fusop_fsyncdir %s %s", path, datasync);
      try
      {
        Handle* handle = (Handle*)fi->fh;
        handle->fsyncdir(datasync);
      }
      catch (Error const& e)
      {
        ELLE_TRACE("Filesystem error on fsyncdir %s: %s", path, e);
        return -e.error_code();
      }
      return 0;
    }

    class FileSystemImpl: public FuseContext
    {
    };

    FileSystem::FileSystem(std::unique_ptr<Operations> op, bool full_tree)
      : _impl(new FileSystemImpl())
      , _operations(std::move(op))
      , _full_tree(full_tree)
    {
      this->_operations->filesystem(this);
      char* journal = getenv("INFINIT_FILESYSTEM_JOURNAL");
      if (journal)
      {
        this->_operations = install_journal(
          std::move(this->_operations), journal);
        this->_operations->filesystem(this);
      }
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
      _where = where.string();
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
      ops.flush = fusop_flush;
      ops.setxattr = fusop_setxattr;
      ops.getxattr = fusop_getxattr;
      ops.listxattr = fusop_listxattr;
      ops.removexattr = fusop_removexattr;
      ops.fsync = fusop_fsync;
      ops.fsyncdir = fusop_fsyncdir;
#if FUSE_VERSION >= 29
      ops.flag_nullpath_ok = true;
#endif
      _impl->create(where.string(), options, &ops, sizeof(ops), this);
      _impl->on_loop_exited([this]
        {
          this->unmount();
        });
      if (!elle::os::getenv("INFINIT_FUSE_POOL", "").empty())
      {
        int nt = 5;
        std::string nthread = elle::os::getenv("INFINIT_FUSE_POOL", "");
        if (!nthread.empty())
          nt = std::stoi(nthread);
        ELLE_TRACE("Pool mode with %s workers", nt);
        _impl->loop_pool(nt);
      }
      else if (!elle::os::getenv("INFINIT_FUSE_THREAD", "").empty())
      {
        ELLE_TRACE("Thread mode");
        _impl->loop_mt();
      }
      else
      {
        ELLE_TRACE("Single mode");
        _impl->loop();
      }
    }

    void
    FileSystem::unmount()
    {
      ELLE_TRACE("unmount");
      if (!_where.empty())
      {
        _where = "";
        _impl->destroy();
        this->_signal();
      }
    }
  }
}
