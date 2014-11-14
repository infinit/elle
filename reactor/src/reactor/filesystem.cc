#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <reactor/filesystem.hh>

#include <elle/assert.hh>

namespace reactor
{
  namespace filesystem
  {
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
    void Path::symlink(boost::filesystem::path const& where){ throw Error(EPERM, "Not implemented");}
    void Path::link(boost::filesystem::path const& where)   { throw Error(EPERM, "Not implemented");}
    void Path::chmod(mode_t mode)                           { throw Error(EPERM, "Not implemented");}
    void Path::chown(uid_t uid, gid_t gid)                  { throw Error(EPERM, "Not implemented");}
    void Path::statfs(struct statvfs * )                    { throw Error(EPERM, "Not implemented");}
    void Path::utimens(const struct timespec tv[2])         { throw Error(EPERM, "Not implemented");}
  }
}