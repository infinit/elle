#include <horizon/linux/Linux.hh>
#include <horizon/linux/FUSE.hh>
#include <horizon/Crux.hh>

#include <Infinit.hh>

#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace linux
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes Linux.
    ///
    elle::Status        Linux::Initialize()
    {
      //
      // initialize FUSE.
      //
      {
        struct ::fuse_operations        operations;

        // set all the pointers to zero.
        ::memset(&operations, 0x0, sizeof (::fuse_operations));

        // operations.statfs: not supported
        operations.getattr = horizon::Crux::getattr;
        operations.fgetattr = horizon::Crux::fgetattr;
        operations.utimens = horizon::Crux::utimens;

        operations.opendir = horizon::Crux::opendir;
        operations.readdir = horizon::Crux::readdir;
        operations.releasedir = horizon::Crux::releasedir;
        operations.mkdir = horizon::Crux::mkdir;
        operations.rmdir = horizon::Crux::rmdir;

        operations.access = horizon::Crux::access;
        operations.chmod = horizon::Crux::chmod;
        operations.chown = horizon::Crux::chown;
#if defined(HAVE_SETXATTR)
        operations.setxattr = horizon::Crux::setxattr;
        operations.getxattr = horizon::Crux::getxattr;
        operations.listxattr = horizon::Crux::listxattr;
        operations.removexattr = horizon::Crux::removexattr;
#endif

        operations.link = horizon::Crux::link;
        operations.readlink = horizon::Crux::readlink;
        operations.symlink = horizon::Crux::symlink;

        operations.create = horizon::Crux::create;
        // operations.mknod: not supported
        operations.open = horizon::Crux::open;
        operations.write = horizon::Crux::write;
        operations.read = horizon::Crux::read;
        operations.truncate = horizon::Crux::truncate;
        operations.ftruncate = horizon::Crux::ftruncate;
        operations.release = horizon::Crux::release;

        operations.rename = horizon::Crux::rename;
        operations.unlink = horizon::Crux::unlink;

        // the following flag being activated prevents the path argument
        // to be passed for functions which take a file descriptor.
        operations.flag_nullpath_ok = 1;

        // initialize FUSE.
        if (FUSE::Initialize(operations) == elle::Status::Error)
          escape("unable to initialize FUSE");
      }
      return elle::Status::Ok;
    }

    ///
    /// this method cleans Linux.
    ///
    elle::Status        Linux::Clean()
    {
      // clean FUSE.
      if (FUSE::Clean() == elle::Status::Error)
        escape("unable to clean FUSE");

      return elle::Status::Ok;
    }

  }
}
