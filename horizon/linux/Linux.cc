//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:31:35 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <horizon/linux/Linux.hh>
#include <horizon/linux/Crux.hh>
#include <horizon/Horizon.hh>

#include <elle/Elle.hh>
#include <agent/Agent.hh>
#include <Infinit.hh>

namespace horizon
{
  namespace linux
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the UID of the 'somebody' user, user which
    /// is used whenever the system cannot map the Infinit user on a local
    /// user.
    ///
    uid_t                               Linux::Somebody::UID;

    ///
    /// this variable contains the GID of the 'somebody' group.
    ///
    gid_t                               Linux::Somebody::GID;

    ///
    /// this varaible contains the mappings between local user/group
    /// identities and Infinit identities.
    ///
    lune::Dictionary                    Linux::Dictionary;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes Linux.
    ///
    elle::Status        Linux::Initialize()
    {
      //
      // initialize the 'somebody' entity.
      //
      {
        struct ::passwd*        passwd;

        // retrieve the passwd structure related to the user 'somebody'.
        // if NULL, try to fallback to 'nobody'.
        if ((passwd = ::getpwnam("somebody")) == NULL &&
            (passwd = ::getpwnam("nobody")) == NULL)
          escape("it seems that the user 'somebody' does not exist");

        // set the uid and gid.
        Linux::Somebody::UID = passwd->pw_uid;
        Linux::Somebody::GID = passwd->pw_gid;
      }

      //
      // load the user/group maps which will be used to translate Infinit
      // user/group identifiers into local identifiers.
      //
      {
        // if the dictionary exist.
        if (Linux::Dictionary.Exist() == elle::StatusTrue)
          {
            // load the dictionary file.
            if (Linux::Dictionary.Load() == elle::StatusError)
              escape("unable to load the dictionary");
          }
      }

      //
      // initialize FUSE.
      //
      {
        struct ::fuse_operations        operations;

        // set all the pointers to zero.
        ::memset(&operations, 0x0, sizeof (::fuse_operations));

        // operations.statfs: not supported
        operations.getattr = Crux::Getattr;
        operations.fgetattr = Crux::Fgetattr;
        operations.utimens = Crux::Utimens;

        operations.opendir = Crux::Opendir;
        operations.readdir = Crux::Readdir;
        operations.releasedir = Crux::Releasedir;
        operations.mkdir = Crux::Mkdir;
        operations.rmdir = Crux::Rmdir;

        operations.access = Crux::Access;
        operations.chmod = Crux::Chmod;
        operations.chown = Crux::Chown;
#ifdef HAVE_SETXATTR
        operations.setxattr = Crux::Setxattr;
        operations.getxattr = Crux::Getxattr;
        operations.listxattr = Crux::Listxattr;
        operations.removexattr = Crux::Removexattr;
#endif

        // operations.link: not supported
        operations.readlink = Crux::Readlink;
        operations.symlink = Crux::Symlink;

        operations.create = Crux::Create;
        // operations.mknod: not supported
        operations.open = Crux::Open;
        operations.write = Crux::Write;
        operations.read = Crux::Read;
        operations.truncate = Crux::Truncate;
        operations.ftruncate = Crux::Ftruncate;
        operations.release = Crux::Release;

        operations.rename = Crux::Rename;
        operations.unlink = Crux::Unlink;

        // the following flag being activated prevents the path argument
        // to be passed for functions which take a file descriptor.
        operations.flag_nullpath_ok = 1;

        // initialize FUSE.
        if (FUSE::Initialize(operations) == elle::StatusError)
          escape("unable to initialize FUSE");
      }
      return elle::StatusOk;
    }

    ///
    /// this method sets up the Linux implementation.
    ///
    elle::Status        Linux::Setup()
    {
      // set up FUSE.
      if (FUSE::Setup(Infinit::Mountpoint) == elle::StatusError)
        escape("unable to set up FUSE");

      return elle::StatusOk;
    }

    ///
    /// this method cleans Linux.
    ///
    elle::Status        Linux::Clean()
    {
      // clean FUSE.
      if (FUSE::Clean() == elle::StatusError)
        escape("unable to clean FUSE");

      return elle::StatusOk;
    }

  }
}
