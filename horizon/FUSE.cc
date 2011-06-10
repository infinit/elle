//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/FUSE.cc
//
// created       julien quintard   [fri jul 31 22:10:21 2009]
// updated       julien quintard   [thu jun  2 15:51:18 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/FUSE.hh>

#include <pig/Crux.hh>

namespace pig
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// the FUSE event broker.
  ///
  elle::Broker*				FUSE::Broker = NULL;

  ///
  /// the FUSE core structure.
  ///
  struct ::fuse*			FUSE::System::Core = NULL;

  ///
  /// this variable holds the path to the mountpoint.
  ///
  char*					FUSE::System::Mountpoint = NULL;

  ///
  /// the FUSE session.
  ///
  struct ::fuse_session*		FUSE::System::Session = NULL;

  ///
  /// the FUSE channel.
  ///
  struct ::fuse_chan*			FUSE::System::Channel = NULL;

  ///
  /// the FUSE buffer size.
  ///
  size_t				FUSE::System::Size;

  ///
  /// the FUSE buffer.
  ///
  char*					FUSE::System::Buffer = NULL;

  ///
  /// this structure contains the function pointers to the FUSE implementation
  /// routines located in the crux.
  ///
  struct ::fuse_operations		FUSE::System::Operations;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the FUSE component.
  ///
  elle::Status		FUSE::Initialize(const elle::String&	mountpoint)
  {
    //
    // build the arguments.
    //
    // note that the -h option can be passed in order to display all
    // the available options including the threaded, debug, file system
    // name, file system type etc.
    //
    // for example the -d option could be used instead of -f in order
    // to activate the debug mode.
    //
    elle::String	ofsname("-ofsname='" +
				Infinit::Descriptor.name +
				"'");
    const char*		arguments[] =
      {
	"PIG",

	"-s",
	"-d",
	"-osubtype='infinit'",
	ofsname.c_str(),
	mountpoint.c_str()
      };

    enter();

    //
    // initialize the FUSE operations.
    //
    {
      // FUSE::System::Operations.statfs: not supported
      FUSE::System::Operations.getattr = Crux::Getattr;
      FUSE::System::Operations.fgetattr = Crux::Fgetattr;
      FUSE::System::Operations.utimens = Crux::Utimens;

      FUSE::System::Operations.opendir = Crux::Opendir;
      FUSE::System::Operations.readdir = Crux::Readdir;
      FUSE::System::Operations.releasedir = Crux::Releasedir;
      FUSE::System::Operations.mkdir = Crux::Mkdir;
      FUSE::System::Operations.rmdir = Crux::Rmdir;

      FUSE::System::Operations.access = Crux::Access;
      FUSE::System::Operations.chmod = Crux::Chmod;
      FUSE::System::Operations.chown = Crux::Chown;
      FUSE::System::Operations.setxattr = Crux::Setxattr;
      FUSE::System::Operations.getxattr = Crux::Getxattr;
      FUSE::System::Operations.listxattr = Crux::Listxattr;
      FUSE::System::Operations.removexattr = Crux::Removexattr;

      FUSE::System::Operations.lock = Crux::Lock;

      // FUSE::System::Operations.link: not supported
      FUSE::System::Operations.readlink = Crux::Readlink;
      FUSE::System::Operations.symlink = Crux::Symlink;

      FUSE::System::Operations.create = Crux::Create;
      // FUSE::System::Operations.mknod: not supported
      FUSE::System::Operations.open = Crux::Open;
      FUSE::System::Operations.write = Crux::Write;
      FUSE::System::Operations.read = Crux::Read;
      FUSE::System::Operations.truncate = Crux::Truncate;
      FUSE::System::Operations.ftruncate = Crux::Ftruncate;
      FUSE::System::Operations.release = Crux::Release;

      FUSE::System::Operations.unlink = Crux::Unlink;
      FUSE::System::Operations.rename = Crux::Rename;

      // FUSE::System::Operations.flush: not supported
      FUSE::System::Operations.fsync = Crux::Fsync;
      FUSE::System::Operations.fsyncdir = Crux::Fsyncdir;

      // XXX pour trouver le bug de compile du kernel linux
      FUSE::System::Operations.flush = Crux::Flush;
    }

    //
    // initialize the FUSE event loop.
    //
    // note that the usual way of setting up FUSE is to call its specific
    // event loop.
    //
    // unfortunately, since elle already provides an event loop, the following
    // uses FUSE low-level functions in order to integrate it into the elle
    // event loop.
    //
    // \todo XXX make it multi-thredable.
    //
    {
      int			multithreaded;

      // setup fuse.
      if ((FUSE::System::Core = ::fuse_setup(
				  (int)(sizeof(arguments) /
					sizeof(elle::Character*)),
				  (char**)arguments,
				  &FUSE::System::Operations,
				  sizeof (FUSE::System::Operations),
				  &FUSE::System::Mountpoint,
				  &multithreaded,
				  NULL)) == NULL)
	escape("unable to setup FUSE");

      // retrieve the FUSE session.
      FUSE::System::Session = FUSE::System::Core->se;

      // retrieve the FUSE channel.
      if ((FUSE::System::Channel =
	   ::fuse_session_next_chan(FUSE::System::Session, NULL)) == NULL)
	escape("unable to retrieve the FUSE channel");

      // retrieve the channel size.
      FUSE::System::Size = ::fuse_chan_bufsize(FUSE::System::Channel);

      if ((FUSE::System::Buffer = (char*)::malloc(FUSE::System::Size)) == NULL)
	escape("unable to allocate the FUSE buffer");
    }

    //
    // create the FUSE event broker.
    //
    {
      elle::Callback<const elle::Natural16>	callback(&FUSE::Event);
      int					fd;

      // retrieve the file descriptor.
      fd = ::fuse_chan_fd(FUSE::System::Channel);

      // allocate the FUSE event broker.
      FUSE::Broker = new elle::Broker((elle::Natural16)fd, callback);

      // activate the broker.
      if (FUSE::Broker->Activate() == elle::StatusError)
	escape("unable to activate the broker");

      // attach the broker to the program's event loop.
      if (elle::Program::Attach(*FUSE::Broker) == elle::StatusError)
	escape("unable to attach the broker to the event loop");
    }

    leave();
  }

  ///
  /// this method cleans the system module.
  ///
  elle::Status		FUSE::Clean()
  {
    enter();

    //
    // destroy the FUSE broker.
    //
    {
      if (FUSE::Broker != NULL)
	{
	  // detach the broker.
	  if (elle::Program::Detach(*FUSE::Broker) == elle::StatusError)
	    escape("unable to detach the broker from the event loop");

	  // deactivate the broker.
	  if (FUSE::Broker->Deactivate() == elle::StatusError)
	    escape("unable to deactivate the broker");

	  // delete the broker.
	  delete FUSE::Broker;
	}
    }

    //
    // release the buffers, sessions etc.
    //
    {
      ::free(FUSE::System::Buffer);

      ::fuse_session_reset(FUSE::System::Session);
    }

    //
    // clean the FUSE event loop.
    //
    {
      // stop FUSE.
      ::fuse_teardown(FUSE::System::Core, FUSE::System::Mountpoint);
    }

    leave();
  }

//
// ---------- static callbacks ------------------------------------------------
//

  ///
  /// this callback is triggered whenever data is available on the
  /// FUSE socket.
  ///
  elle::Status		FUSE::Event(const elle::Natural16&)
  {
    struct ::fuse_chan*	channel = FUSE::System::Channel;
    int			res;

    enter();

    // retrieve the upcall from the kernel through the FUSE channel.
    res = ::fuse_chan_recv(&channel,
			   FUSE::System::Buffer,
			   FUSE::System::Size);

    // retry later if necessary.
    if (res == -EINTR)
      leave();

    // exit if an error occured.
    if (res <= 0)
      {
	if (elle::Program::Exit() == elle::StatusError)
	  escape("unable to exit the program");
      }

    // finally, trigger the upcall handler.
    ::fuse_session_process(FUSE::System::Session,
			   FUSE::System::Buffer,
			   res,
			   channel);

    leave();
  }

}
