#include <horizon/macosx/Crux.hh>
#include <horizon/Crux.hh>

#include <elle/log.hh>

#include <elle/idiom/Close.hh>
# include <sys/statvfs.h>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.horizon.macosx.Crux");

namespace horizon
{
  namespace macosx
  {

//
// ---------- callbacks -------------------------------------------------------
//

    int                 Crux::statfs(const char*,
                                     struct ::statvfs* statvfs)
    {
      ELLE_TRACE_SCOPE("%s()", __FUNCTION__);

      // XXX[these numbers are based on the return value of statvfs() on
      //     a local file system; just to make sure the Finder does not
      //     complain there is not storage space left]
      statvfs->f_bsize = 1048576;
      statvfs->f_frsize = 4096;
      statvfs->f_blocks = 29412370;
      statvfs->f_bfree = 1096215;
      statvfs->f_bavail = 1032215;
      statvfs->f_files = 29412368;
      statvfs->f_ffree = 1032215;
      statvfs->f_favail = 1032215;
      statvfs->f_fsid = 234881026;
      statvfs->f_flag = ST_NOSUID;
      statvfs->f_namemax = 255;
      // XXX

      return (0);
    }

#if defined(HAVE_SETXATTR)
    int
    Crux::setxattr(const char* path,
                   const char* name,
                   const char* value,
                   size_t size,
                   int flags,
                   uint32_t)
    {
      return (horizon::Crux::setxattr(path, name, value, size, flags));
    }

    int
    Crux::getxattr(const char* path,
                   const char* name,
                   char* value,
                   size_t size,
                   uint32_t)
    {
      return (horizon::Crux::getxattr(path, name, value, size));
    }
#endif

  }
}
