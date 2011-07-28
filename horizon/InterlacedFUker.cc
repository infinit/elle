//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/InterlacedFUker.cc
//
// created       julien quintard   [tue jul 26 15:33:54 2011]
// updated       julien quintard   [wed jul 27 08:20:44 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/InterlacedFUker.hh>
#include <pig/FUSE.hh>

#include <hole/Hole.hh>

namespace pig
{

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  InterlacedFUker::InterlacedFUker():
    broker(NULL),
    fuse(NULL),
    mountpoint(NULL),
    session(NULL),
    channel(NULL),
    size(0)
  {
  }

  ///
  /// destructor.
  ///
  InterlacedFUker::~InterlacedFUker()
  {
    //
    // destroy the FUSE broker.
    //
    {
      if (this->broker != NULL)
	{
	  // detach the broker.
	  elle::Program::Detach(*this->broker);

	  // deactivate the broker.
	  this->broker->Deactivate();

	  // delete the broker.
	  delete this->broker;
	}
    }

    //
    // release the buffers, sessions etc.
    //
    {
      // release the session.
      if (this->session != NULL)
	::fuse_session_reset(this->session);
    }

    //
    // clean the FUSE event loop.
    //
    {
      // stop FUSE.
      if (this->fuse != NULL)
	::fuse_teardown(this->fuse, this->mountpoint);
    }
  }

//
// ---------- fuker -----------------------------------------------------------
//

  ///
  /// XXX
  ///
  elle::Status		InterlacedFUker::Setup()
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
				hole::Hole::Descriptor.name +
				"'");
    const char*		arguments[] =
      {
	"PIG",

	"-s",
	"-f",
	"-osubtype='infinit'",
	ofsname.c_str(),
	FUSE::Mountpoint.c_str()
      };

    enter();

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
      if ((this->fuse = ::fuse_setup(
			  (int)(sizeof (arguments) /
				sizeof (elle::Character*)),
			  (char**)arguments,
			  &FUSE::Operations,
			  sizeof (FUSE::Operations),
			  &this->mountpoint,
			  &multithreaded,
			  NULL)) == NULL)
	escape("unable to setup FUSE");

      // retrieve the FUSE session.
      this->session = this->fuse->se;

      // retrieve the FUSE channel.
      if ((this->channel =
	   ::fuse_session_next_chan(this->session, NULL)) == NULL)
	escape("unable to retrieve the FUSE channel");

      // retrieve the channel size.
      this->size = ::fuse_chan_bufsize(this->channel);
    }

    //
    // create the FUSE event broker.
    //
    {
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  const elle::Natural16> >	callback(&InterlacedFUker::Event,
						 this);
      int				fd;

      // retrieve the file descriptor.
      fd = ::fuse_chan_fd(this->channel);

      // allocate the FUSE event broker.
      this->broker =
	new elle::Broker((elle::Natural16)fd, callback);

      // activate the broker.
      if (this->broker->Activate() == elle::StatusError)
	escape("unable to activate the broker");

      // attach the broker to the program's event loop.
      if (elle::Program::Attach(*this->broker) == elle::StatusError)
	escape("unable to attach the broker to the event loop");
    }

    leave();
  }

//
// ---------- callbacks -------------------------------------------------------
//

  ///
  /// this callback is triggered whenever data is available on the
  /// FUSE socket.
  ///
  elle::Status		InterlacedFUker::Event(const elle::Natural16&)
  {
    struct ::fuse_chan*	channel = this->channel;
    char*		buffer;
    int			res;

    enter(slab(buffer, ::free));

    // allocate a buffer.
    /// \todo XXX this could probably be optimised in order to allocate
    /// a buffer only if an event is already being processed!
    if ((buffer = (char*)::malloc(this->size)) == NULL)
      escape("unable to allocate a FUSE buffer");

    // retrieve the upcall from the kernel through the FUSE channel.
    res = ::fuse_chan_recv(&channel,
			   buffer,
			   this->size);

    // retry later if necessary.
    if (res == -EINTR)
      leave();

    // exit if an error occured.
    if (res <= 0)
      {
	if (elle::Program::Exit() == elle::StatusError)
	  escape("unable to exit the program");
      }

    // trigger the upcall handler.
    ::fuse_session_process(this->session,
			   buffer,
			   res,
			   channel);

    // release the buffer.
    ::free(buffer);

    // waive the tracking.
    waive(buffer);

    leave();
  }

}
