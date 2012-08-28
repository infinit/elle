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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the UID of the 'somebody' user, user which
    /// is used whenever the system cannot map the Infinit user on a local
    /// user.
    ///
    uid_t                               MacOSX::Somebody::UID;

    ///
    /// this variable contains the GID of the 'somebody' group.
    ///
    gid_t                               MacOSX::Somebody::GID;

    ///
    /// this varaible contains the mappings between local user/group
    /// identities and Infinit identities.
    ///
    lune::Dictionary                    MacOSX::Dictionary;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes MacOS X.
    ///
    elle::Status        MacOSX::Initialize()
    {
      //
      // initialize the 'somebody' entity.
      //
      {
        struct ::passwd*        passwd;

        // retrieve the passwd structure related to the user 'somebody'.
        // if nullptr, try to fallback to 'nobody'.
        if ((passwd = ::getpwnam("somebody")) == nullptr &&
            (passwd = ::getpwnam("nobody")) == nullptr)
          escape("it seems that the user 'somebody' does not exist");

        // set the uid and gid.
        MacOSX::Somebody::UID = passwd->pw_uid;
        MacOSX::Somebody::GID = passwd->pw_gid;
      }

      //
      // load the user/group maps which will be used to translate Infinit
      // user/group identifiers into local identifiers.
      //
      {
        // if the dictionary exist.
        if (lune::Dictionary::exists(Infinit::User) == true)
          MacOSX::Dictionary.load(Infinit::User);
      }

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
