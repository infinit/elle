#define _FILE_OFFSET_BITS 64
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>

#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/reactor/filesystem.hh>

ELLE_LOG_COMPONENT("elle.reactor.filesystem");

namespace elle
{
  namespace reactor
  {
    namespace filesystem
    {
      static std::string normalize(std::string path)
      {
        if (path == "" || path == "\\")
          path = "/";
        // normalize
        auto pos = path.find_first_of("\\");
        while (pos != path.npos)
        {
          path[pos] = '/';
          pos = path.find_first_of("\\");
        }
        return path;
      }

      Operations::Operations()
        : _filesystem(nullptr)
      {}

      bool
      FileSystem::_wait(Thread* thread, Waker const& waker)
      {
        if (this->_impl)
          return Waitable::_wait(thread, waker);
        else
          return false;
      }

      std::shared_ptr<Path>
      FileSystem::fetch_recurse(std::string path)
      {
        path = normalize(path);
        ELLE_DEBUG_SCOPE("%s: fetch_recurse \"%s\"", *this, path);
        ELLE_DUMP("from %s", _cache);
        auto it = _cache.find(path);
        if (it != _cache.end())
        {
          ELLE_DEBUG("%s: hit on '%s': %s", *this, path, it->second.get());
          return it->second;
        }
        else
        {
          ELLE_DEBUG("%s: miss on %s", *this, path);
          if (path == "/")
          {
            ELLE_DEBUG("%s: root fetch", *this);
            auto p = _operations->path("/");
            if (p->allow_cache())
              _cache[path] = p;
            return p;
          }
          auto bpath = bfs::path(path);
          auto parent = this->fetch_recurse(bpath.parent_path().string());
          auto p = parent->child(bpath.filename().string());
          if (p->allow_cache())
            _cache[path] = p;
          return p;
        }
      }

      std::shared_ptr<Path>
      FileSystem::path(std::string const& opath)
      {
        ELLE_DEBUG_SCOPE("%s: fetch \"%s\"", *this, opath);
        auto spath = normalize(opath);
        ELLE_ASSERT(_impl);
        if (this->_full_tree)
        {
          auto res = this->fetch_recurse(spath);
          return res->unwrap();
        }
        else
        {
          auto it = this->_cache.find(spath);
          if (it == this->_cache.end())
          {
            auto res = this->_operations->path(spath);
            if (res->allow_cache())
              this->_cache[spath] = res;
            return res;
          }
          else
            return it->second;
        }
      }

      std::shared_ptr<Path>
      FileSystem::extract(std::string const& path_)
      {
        auto path = normalize(path_);
        auto it = this->_cache.find(path);
        if (it == this->_cache.end())
          return {};
        auto res = it->second;
        this->_cache.erase(path);
        return res->unwrap();
      }

      std::shared_ptr<Path>
      FileSystem::set(std::string const& path_,
                      std::shared_ptr<Path> new_content)
      {
        auto path = normalize(path_);
        std::shared_ptr<Path> res = extract(path);
        this->_cache[path] =
          this->_operations->wrap(path, new_content);
        return res;
      }

      std::shared_ptr<Path>
      FileSystem::get(std::string const& path_)
      {
        auto path = normalize(path_);
        auto it = this->_cache.find(path);
        if (it == this->_cache.end())
          return nullptr;
        else
          return it->second;
      }

      std::unique_ptr<Handle>
      Path::create(int flags, mode_t mode)
      {
        ELLE_ASSERT(flags & O_CREAT);
        return this->open(flags, mode);
      }

      void
      Path::unlink()
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::rmdir()
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::mkdir(mode_t)
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::rename(bfs::path const& p)
      {
        throw Error(EPERM, "Not implemented");
      }

      bfs::path
      Path::readlink()
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::statfs(struct statvfs* fs)
      {
        // The fuse mount procedures in some case needs statvfs(/) to work.
        memset(fs, 0, sizeof(struct statvfs));
        fs->f_namemax = 256;
        fs->f_fsid = 0;
        fs->f_frsize = 4096;
        fs->f_bsize = 4096;
        fs->f_flag = 0;
      }

      void
      Path::symlink(bfs::path const& where)
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::link(bfs::path const& where)
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::chmod(mode_t mode)
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::chown(int uid, int gid)
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::utimens(const struct timespec tv[2])
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::truncate(off_t new_size)
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Handle::ftruncate(off_t new_size)
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::setxattr(std::string const& name, std::string const& value,
                     int flags)
      {
        throw Error(EPERM, "Not implemented");
      }

      std::string
      Path::getxattr(std::string const& name)
      {
        throw Error(EPERM, "Not implemented");
      }

      std::vector<std::string>
      Path::listxattr()
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Path::removexattr(std::string const& name)
      {
        throw Error(EPERM, "Not implemented");
      }

      void
      Handle::fsync(int datasync)
      {}

      void
      Handle::fsyncdir(int datasync)
      {}

      BindOperations::BindOperations(bfs::path  source)
        : _source(std::move(source))
      {}

      BindPath::BindPath(bfs::path const& path, BindOperations& ops)
        : _where(ops.source() / path)
        , _ops(ops)
      {}

      void
      BindPath::stat(struct stat* st)
      {
        int res = ::stat(this->_where.string().c_str(), st);
        if (res < 0)
          throw Error(errno, strerror(errno));
      }

      void
      BindPath::list_directory(OnDirectoryEntry cb)
      {
        bfs::directory_iterator it(this->_where);
        for(;it != bfs::directory_iterator(); ++it)
        {
          cb(it->path().filename().string(), nullptr);
        }
      }

      std::unique_ptr<Handle>
      BindPath::open(int flags, mode_t mode)
      {
        int fd = ::open(this->_where.string().c_str(), flags, mode);
        return make_handle(this->_where, fd);
      }

      void
      BindPath::unlink()
      {
        boost::system::error_code erc;
        bfs::remove(this->_where, erc);
        if (erc)
          throw Error(erc.value(), erc.message());
      }

      void
      BindPath::mkdir(mode_t mode)
      {
        boost::system::error_code erc;
        bfs::create_directory(this->_where, erc);
        if (erc)
          throw Error(erc.value(), erc.message());
      }

      void
      BindPath::rmdir()
      {
        boost::system::error_code erc;
        if (bfs::is_directory(this->_where, erc))
        {
          bfs::remove(this->_where, erc);
          if (erc)
            throw Error(erc.value(), erc.message());
        }
        else
          throw Error(ENOTDIR, "Not a directory");
      }

      void
      BindPath::rename(bfs::path const& localtarget)
      {
        bfs::path target = this->ops().source() / localtarget;
        boost::system::error_code erc;
        bfs::rename(this->_where, target, erc);
        if (erc)
          throw Error(erc.value(), erc.message());
      }

      bfs::path
      BindPath::readlink()
      {
        boost::system::error_code erc;
        bfs::path target = bfs::read_symlink(this->_where, erc);
        if (erc)
          throw Error(erc.value(), erc.message());
        return target;
      }

      void
      BindPath::symlink(bfs::path const& target)
      {
        boost::system::error_code erc;
        bfs::create_symlink(target, this->_where, erc);
        if (erc)
          throw Error(erc.value(), erc.message());
      }

      void
      BindPath::link(bfs::path const& target)
      {
        boost::system::error_code erc;
        bfs::create_hard_link(target, this->_where, erc);
        if (erc)
          throw Error(erc.value(), erc.message());
      }

      void
      BindPath::chmod(mode_t mode)
      {
        boost::system::error_code erc;
        bfs::permissions(this->_where, (bfs::perms)mode, erc);
        if (erc)
          throw Error(erc.value(), erc.message());
      }

      void
      BindPath::chown(int uid, int gid)
      {
  #ifdef INFINIT_WINDOWS
        throw Error(EPERM, "Not implemented");
  #else
        int res = ::chown(this->_where.string().c_str(), uid, gid);
        if (res < 0)
          throw Error(errno, strerror(errno));
  #endif
      }

      void
      BindPath::statfs(struct statvfs *s)
      {
  #if defined(INFINIT_WINDOWS) || defined(INFINIT_ANDROID)
        throw Error(EPERM, "Not implemented");
  #else
       int res = ::statvfs(this->_where.string().c_str(), s);
       if (res < 0)
         throw Error(errno, strerror(errno));
  #endif
      }

      void
      BindPath::utimens(const struct timespec tv[2])
      {
  #ifdef INFINIT_LINUX
        int res = ::utimensat(0, this->_where.string().c_str(), tv, 0);
        if (res < 0)
          throw Error(errno, strerror(errno));
  #else
        throw Error(EPERM, "Not implemented");
  #endif
      }

      void
      BindPath::truncate(off_t new_size)
      {
        boost::system::error_code erc;
        bfs::resize_file(this->_where, new_size, erc);
        if (erc)
          throw Error(erc.value(), erc.message());
      }

      /// Return a Path for given child name.
      std::shared_ptr<Path>
      BindPath::child(std::string const& name)
      {
        return std::make_shared<BindPath>(this->_where / name, this->ops());
      }

      std::unique_ptr<BindHandle>
      BindPath::make_handle(bfs::path& where, int fd)
      {
        return std::make_unique<BindHandle>(fd, where);
      }

      BindHandle::BindHandle(int fd, bfs::path  where)
        : _fd(fd)
        , _where(std::move(where))
      {}

      void
      BindHandle::close()
      {
        ::close(this->_fd);
      }

      int
      BindHandle::read(elle::WeakBuffer buffer, size_t size, off_t offset)
      {
        lseek(this->_fd, offset, SEEK_SET);
        return ::read(this->_fd, (void*)buffer.contents(), size);
      }

      int
      BindHandle::write(elle::ConstWeakBuffer buffer, size_t size, off_t offset)
      {
        lseek(this->_fd, offset, SEEK_SET);
        return ::write(this->_fd, buffer.contents(), size);
      }

      std::shared_ptr<Path>
      BindOperations::path(std::string const& p)
      {
        return std::make_shared<BindPath>(p, *this);
      }

      void BindHandle::ftruncate(off_t sz)
      {
        int res = ::ftruncate(this->_fd, sz);
        if (res != 0)
          throw Error(errno, strerror(errno));
      }
    }
  }
}
