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
  }
}