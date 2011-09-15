//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien quintard   [fri jul 31 22:10:21 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/FUSE.hh>
#include <pig/Crux.hh>
#include <pig/InterlacedFUker.hh>
#include <pig/ParallelFUker.hh>
#include <pig/SequentialFUker.hh>

#include <Infinit.hh>

namespace pig
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// the FUSE mountpoint.
  ///
  elle::String				FUSE::Mountpoint;

  ///
  /// the FUSE-Broker i.e FUker.
  ///
  FUker*				FUSE::Fuker = NULL;

  ///
  /// this structure contains the function pointers to the FUSE implementation
  /// routines located in the crux.
  ///
  struct ::fuse_operations		FUSE::Operations;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the FUSE component.
  ///
  elle::Status		FUSE::Initialize()
  {
    enter();

    //
    // initialize the FUSE operations.
    //
    {
      // set all the pointers to zero.
      ::memset(&FUSE::Operations, 0x0, sizeof (::fuse_operations));

      // FUSE::Operations.statfs: not supported
      FUSE::Operations.getattr = Crux::Getattr;
      FUSE::Operations.fgetattr = Crux::Fgetattr;
      FUSE::Operations.utimens = Crux::Utimens;

      FUSE::Operations.opendir = Crux::Opendir;
      FUSE::Operations.readdir = Crux::Readdir;
      FUSE::Operations.releasedir = Crux::Releasedir;
      FUSE::Operations.mkdir = Crux::Mkdir;
      FUSE::Operations.rmdir = Crux::Rmdir;

      FUSE::Operations.access = Crux::Access;
      FUSE::Operations.chmod = Crux::Chmod;
      FUSE::Operations.chown = Crux::Chown;
#ifdef HAVE_SETXATTR
      FUSE::Operations.setxattr = Crux::Setxattr;
      FUSE::Operations.getxattr = Crux::Getxattr;
      FUSE::Operations.listxattr = Crux::Listxattr;
      FUSE::Operations.removexattr = Crux::Removexattr;
#endif

      // FUSE::Operations.link: not supported
      FUSE::Operations.readlink = Crux::Readlink;
      FUSE::Operations.symlink = Crux::Symlink;

      FUSE::Operations.create = Crux::Create;
      // FUSE::Operations.mknod: not supported
      FUSE::Operations.open = Crux::Open;
      FUSE::Operations.write = Crux::Write;
      FUSE::Operations.read = Crux::Read;
      FUSE::Operations.truncate = Crux::Truncate;
      FUSE::Operations.ftruncate = Crux::Ftruncate;
      FUSE::Operations.release = Crux::Release;

      FUSE::Operations.rename = Crux::Rename;
      FUSE::Operations.unlink = Crux::Unlink;

      // the following flag being activated prevents the path argument
      // to be passed for functions which take a file descriptor.
      FUSE::Operations.flag_nullpath_ok = 1;
    }

    //
    // allocate the FUker according to the configuration
    //
    {
      // according to the configuration...
      switch (Infinit::Configuration.fuse.fuker)
	{
	case FUker::TypeSequential:
	  {
	    // allocate the sequential FUker.
	    FUSE::Fuker = new SequentialFUker;

	    break;
	  }
	case FUker::TypeInterlaced:
	  {
	    // allocate the interlaced FUker.
	    FUSE::Fuker = new InterlacedFUker;

	    break;
	  }
	case FUker::TypeParallel:
	  {
	    // allocate the parallel FUker.
	    FUSE::Fuker = new ParallelFUker;

	    break;
	  }
	default:
	  {
	    escape("unknown fuker '%u'",
		   Infinit::Configuration.fuse.fuker);
	  }
	}
    }

    leave();
  }

  ///
  /// this methods sets up the FUSE module.
  ///
  elle::Status		FUSE::Setup(const elle::String&		mountpoint)
  {
    enter();

    // set the mountpoint.
    FUSE::Mountpoint = mountpoint;

    // set up the FUker.
    if (FUSE::Fuker->Setup() == elle::StatusError)
      escape("unable to set up the FUker");

    leave();
  }

  ///
  /// this method cleans the system module.
  ///
  elle::Status		FUSE::Clean()
  {
    enter();

    // delete the FUker.
    delete FUSE::Fuker;

    leave();
  }

}
