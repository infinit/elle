//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:31:35 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <facade/unix/PIG.hh>
#include <facade/unix/Crux.hh>

#include <elle/Elle.hh>
#include <agent/Agent.hh>

namespace facade
{
  namespace unix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the UID of the 'somebody' user, user which
    /// is used whenever the system cannot map the Infinit user on a local
    /// user.
    ///
    uid_t				PIG::Somebody::UID;

    ///
    /// this variable contains the GID of the 'somebody' group.
    ///
    gid_t				PIG::Somebody::GID;

    ///
    /// this varaible contains the mappings between local user/group
    /// identities and Infinit identities.
    ///
    lune::Dictionary			PIG::Dictionary;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes PIG.
    ///
    elle::Status	PIG::Initialize()
    {
      elle::String	mountpoint;

      enter();

      //
      // initialize the 'somebody' entity.
      //
      {
	struct ::passwd*	passwd;

	// retrieve the passwd structure related to the user 'somebody'.
	// if NULL, try to fallback to 'nobody'.
	if ((passwd = ::getpwnam("somebody")) == NULL &&
	    (passwd = ::getpwnam("nobody")) == NULL)
	  escape("it seems that the user 'somebody' does not exist");

	// set the uid and gid.
	PIG::Somebody::UID = passwd->pw_uid;
	PIG::Somebody::GID = passwd->pw_gid;
      }

      //
      // load the user/group maps which will be used to translate Infinit
      // user/group identifiers into local identifiers.
      //
      {
	// if the dictionary exist.
	if (PIG::Dictionary.Exist(agent::Agent::Identity.name) ==
	    elle::StatusTrue)
	  {
	    // load the dictionary file.
	    if (PIG::Dictionary.Load(agent::Agent::Identity.name) ==
		elle::StatusError)
	      escape("unable to load the dictionary");
	  }
      }

      //
      // initialize FUSE.
      //
      {
	struct ::fuse_operations	operations;

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

	if (FUSE::Initialize(operations) == elle::StatusError)
	  escape("unable to initialize FUSE");
      }

      //
      // set up FUSE.
      //
      {
	// XXX everything must change!
	if (Infinit::Parser->Test("Mountpoint") == elle::StatusFalse)
	  {
	    char		command[256];

	    mountpoint =
	      elle::System::Path::Home + "/local/mnt/infinit/personal";

	    printf("Infinit is about to be mounted at '%s'\n",
		   mountpoint.c_str());

	    sprintf(command, "mkdir -p %s",
		    mountpoint.c_str());
	    system(command);
	  }
	else
	  {
	    // retrieve the mount point.
	    if (Infinit::Parser->Value("Mountpoint",
				       mountpoint) == elle::StatusError)
	      {
		// display the usage.
		Infinit::Parser->Usage();

		escape("unable to retrieve the mount point");
	      }
	  }

	if (FUSE::Setup(mountpoint) == elle::StatusError)
	  escape("unable to set up FUSE");
      }

      leave();
    }

    ///
    /// this method cleans PIG.
    ///
    elle::Status	PIG::Clean()
    {
      enter();

      // clean FUSE.
      if (FUSE::Clean() == elle::StatusError)
	escape("unable to clean FUSE");

      leave();
    }

    ///
    /// this method sets up the pig-specific options.
    ///
    elle::Status	PIG::Options()
    {
      enter();

      // register the option.
      if (Infinit::Parser->Register(
	    "Mountpoint",
	    'm',
	    "mountpoint",
	    "specifies the mount point",
	    elle::Parser::KindRequired) == elle::StatusError)
	escape("unable to register the option");

      leave();
    }

  }
}
