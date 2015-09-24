#define _FILE_OFFSET_BITS 64
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>

#include <reactor/filesystem.hh>

#include <elle/assert.hh>

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
    Operations::Operations()
    : _filesystem(nullptr)
    {
    }

    std::unique_ptr<Handle> Path::create(int flags, mode_t mode)
    {
      ELLE_ASSERT(flags & O_CREAT);
      return this->open(flags, mode);
    }
    void Path::unlink()
    {
      throw Error(EPERM, "Not implemented");
    }
    void Path::rmdir()
    {
      throw Error(EPERM, "Not implemented");
    }
    void Path::mkdir(mode_t)
    {
      throw Error(EPERM, "Not implemented");
    }
    void Path::rename(boost::filesystem::path const& p)
    {
      throw Error(EPERM, "Not implemented");
    }
    boost::filesystem::path Path::readlink()
    {
      throw Error(EPERM, "Not implemented");
    }
    void Path::statfs(struct statvfs * fs)
    { // The fuse mount procedures in some case needs statvfs(/) to work.
      memset(fs, 0, sizeof(struct statvfs));
      fs->f_namemax = 256;
      fs->f_fsid = 0;
      fs->f_frsize = 4096;
      fs->f_bsize = 4096;
      fs->f_flag = 0;
    }
    void Path::symlink(boost::filesystem::path const& where){ throw Error(EPERM, "Not implemented");}
    void Path::link(boost::filesystem::path const& where)   { throw Error(EPERM, "Not implemented");}
    void Path::chmod(mode_t mode)                           { throw Error(EPERM, "Not implemented");}
    void Path::chown(int uid, int gid)                      { throw Error(EPERM, "Not implemented");}
    void Path::utimens(const struct timespec tv[2])         { throw Error(EPERM, "Not implemented");}
    void Path::truncate(off_t new_size)                     { throw Error(EPERM, "Not implemented");}
    void Handle::ftruncate(off_t new_size)                  { throw Error(EPERM, "Not implemented");}
    void Path::setxattr(std::string const& name, std::string const& value, int flags) { throw Error(EPERM, "Not implemented");}
    std::string Path::getxattr(std::string const& name) { throw Error(EPERM, "Not implemented");}
    std::vector<std::string> Path::listxattr()          { throw Error(EPERM, "Not implemented");}
    void Path::removexattr(std::string const& name)     { throw Error(EPERM, "Not implemented");}
    void Handle::fsync(int datasync) {}
    void Handle::fsyncdir(int datasync) {}

    namespace bfs = boost::filesystem;

    BindOperations::BindOperations(boost::filesystem::path const& source)
    : _source(source)
    {
    }

    BindPath::BindPath(bfs::path const&path, BindOperations& ops)
    : _where(ops.source() / path)
    , _ops(ops)
    {
    }

    void BindPath::stat(struct stat* st)
    {
      int res = ::stat(_where.string().c_str(), st);
      if (res < 0)
        throw Error(errno, strerror(errno));
    }
    void BindPath::list_directory(OnDirectoryEntry cb)
    {
      bfs::directory_iterator it(_where);
      for(;it != bfs::directory_iterator(); ++it)
      {
        cb(it->path().filename().string(), nullptr);
      }
    }
    std::unique_ptr<Handle> BindPath::open(int flags, mode_t mode)
    {
      int fd = ::open(_where.string().c_str(), flags, mode);
      return make_handle(_where, fd);
    }
    void BindPath::unlink()
    {
      boost::system::error_code erc;
      bfs::remove(_where, erc);
      if (erc)
        throw Error(erc.value(), erc.message());
    }
    void BindPath::mkdir(mode_t mode)
    {
      boost::system::error_code erc;
      bfs::create_directory(_where, erc);
      if (erc)
        throw Error(erc.value(), erc.message());
    }
    void BindPath::rmdir()
    {
      boost::system::error_code erc;
      if (bfs::is_directory(_where, erc))
      {
        bfs::remove(_where, erc);
        if (erc)
          throw Error(erc.value(), erc.message());
      }
      else
        throw Error(ENOTDIR, "Not a directory");
    }
    void BindPath::rename(boost::filesystem::path const& localtarget)
    {
      boost::filesystem::path target = ops().source() / localtarget;
      boost::system::error_code erc;
      bfs::rename(_where, target, erc);
      if (erc)
      {
        throw Error(erc.value(), erc.message());
      }
    }

    boost::filesystem::path BindPath::readlink()
    {
      boost::system::error_code erc;
      bfs::path target = bfs::read_symlink(_where, erc);
      if (erc)
        throw Error(erc.value(), erc.message());
      return target;
    }
    void BindPath::symlink(boost::filesystem::path const& target)
    {
      boost::system::error_code erc;
      bfs::create_symlink(target, _where, erc);
      if (erc)
        throw Error(erc.value(), erc.message());
    }
    void BindPath::link(boost::filesystem::path const& target)
    {
      boost::system::error_code erc;
      bfs::create_hard_link(target, _where, erc);
      if (erc)
        throw Error(erc.value(), erc.message());
    }
    void BindPath::chmod(mode_t mode)
    {
      boost::system::error_code erc;
      bfs::permissions(_where, (bfs::perms)mode, erc);
      if (erc)
        throw Error(erc.value(), erc.message());
    }
    void BindPath::chown(int uid, int gid)
    {
      #ifdef INFINIT_WINDOWS
      throw Error(EPERM, "Not implemented");
      #else
      int res = ::chown(_where.string().c_str(), uid, gid);
      if (res < 0)
        throw Error(errno, strerror(errno));
      #endif
    }
    void BindPath::statfs(struct statvfs *s)
    {
      #if defined(INFINIT_WINDOWS) || defined(INFINIT_ANDROID)
      throw Error(EPERM, "Not implemented");
      #else
     int res = ::statvfs(_where.string().c_str(), s);
     if (res < 0)
       throw Error(errno, strerror(errno));
     #endif
    }
    void BindPath::utimens(const struct timespec tv[2])
    {
#ifdef INFINIT_LINUX
      int res = ::utimensat(0, _where.string().c_str(), tv, 0);
      if (res < 0)
        throw Error(errno, strerror(errno));
#else
      throw Error(EPERM, "Not implemented");
#endif
    }
    void BindPath::truncate(off_t new_size)
    {
      boost::system::error_code erc;
      boost::filesystem::resize_file(_where, new_size, erc);
      if (erc)
        throw Error(erc.value(), erc.message());
    }
    /// Return a Path for given child name.
    std::shared_ptr<Path> BindPath::child(std::string const& name)
    {
      return std::make_shared<BindPath>(_where / name, ops());
    }
    std::unique_ptr<BindHandle> BindPath::make_handle(boost::filesystem::path& where,
                                                      int fd)
    {
      return elle::make_unique<BindHandle>(fd, where);
    }

    BindHandle::BindHandle(int fd, boost::filesystem::path const& where)
    : _fd(fd)
    , _where(where)
    {
    }
    void BindHandle::close()
    {
      ::close(_fd);
    }
    int BindHandle::read(elle::WeakBuffer buffer, size_t size, off_t offset)
    {
      lseek(_fd, offset, SEEK_SET);
      return ::read(_fd, (void*)buffer.contents(), size);
    }
    int BindHandle::write(elle::WeakBuffer buffer, size_t size, off_t offset)
    {
      lseek(_fd, offset, SEEK_SET);
      return ::write(_fd, buffer.contents(), size);
    }

    std::shared_ptr<Path> BindOperations::path(std::string const& p)
    {
      return std::make_shared<BindPath>(p, *this);
    }

    void BindHandle::ftruncate(off_t sz)
    {
      int res = ::ftruncate(_fd, sz);
      if (res != 0)
        throw Error(errno, strerror(errno));
    }
  }
}
