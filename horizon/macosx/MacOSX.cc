#include <horizon/macosx/MacOSX.hh>
#include <horizon/macosx/Crux.hh>
#include <horizon/macosx/FUSE.hh>

#include <Infinit.hh>

#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes MacOS X.
    ///
    elle::Status        MacOSX::Initialize()
    {
      //
      // initialize FUSE.
      //
      {
        struct ::fuse_operations        operations;

        // set all the pointers to zero.
        ::memset(&operations, 0x0, sizeof (::fuse_operations));

        operations.statfs = Crux::statfs;
        operations.getattr = Crux::getattr;
        operations.fgetattr = Crux::fgetattr;
        operations.utimens = Crux::utimens;

        operations.opendir = Crux::opendir;
        operations.readdir = Crux::readdir;
        operations.releasedir = Crux::releasedir;
        operations.mkdir = Crux::mkdir;
        operations.rmdir = Crux::rmdir;

        operations.access = Crux::access;
        operations.chmod = Crux::chmod;
        operations.chown = Crux::chown;
#if defined(HAVE_SETXATTR)
        operations.setxattr = Crux::setxattr;
        operations.getxattr = Crux::getxattr;
        operations.listxattr = Crux::listxattr;
        operations.removexattr = Crux::removexattr;
#endif

        operations.link = Crux::link;
        operations.readlink = Crux::readlink;
        operations.symlink = Crux::symlink;

        operations.create = Crux::create;
        // operations.mknod: not supported
        operations.open = Crux::open;
        operations.write = Crux::write;
        operations.read = Crux::read;
        operations.truncate = Crux::truncate;
        operations.ftruncate = Crux::ftruncate;
        operations.release = Crux::release;

        operations.rename = Crux::rename;
        operations.unlink = Crux::unlink;

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
    /// this method cleans MacOS X.
    ///
    elle::Status        MacOSX::Clean()
    {
      // clean FUSE.
      if (FUSE::Clean() == elle::Status::Error)
        escape("unable to clean FUSE");

      return elle::Status::Ok;
    }

  }
}
