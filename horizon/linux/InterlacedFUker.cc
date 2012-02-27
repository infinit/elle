//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [tue jul 26 15:33:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <horizon/linux/InterlacedFUker.hh>
#include <horizon/linux/FUSE.hh>

#include <hole/Hole.hh>

namespace horizon
{
  namespace linux
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
    /// this method sets up the fuker.
    ///
    elle::Status        InterlacedFUker::Setup()
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
      elle::String      ofsname("-ofsname=" +
                                hole::Hole::Descriptor.name);
      const char*       arguments[] =
        {
          "horizon",

          //
          // this option disables multi-threading.
          //
          "-s",

          //
          // this option does not register FUSE as a daemon but
          // run it in foreground.
          //
          "-f",

          //
          // this option indicates the name of the file system type.
          //
          "-osubtype=infinit",

          //
          // this option disables remote file locking.
          //
          "-ono_remote_lock",

          //
          // this option indicates the kernel to perform reads
          // through large chunks.
          //
          "-olarge_read",

          //
          // this option indicates the kernel to perform writes
          // through big writes.
          //
          "-obig_writes",

          //
          // this option activates the in-kernel caching based on
          // the modification times.
          //
          "-oauto_cache",

          //
          // this option indicates the kernel to always forward the I/Os
          // to the filesystem.
          //
          "-odirect_io",

          //
          // this option specifies the name of the file system instance.
          //
          ofsname.c_str(),

          //
          // and finally, the mountpoint.
          //
          FUSE::Mountpoint.c_str()
        };

      //
      // initialize the FUSE event loop.
      //
      // note that the usual way of setting up FUSE is to call its specific
      // event loop.
      //
      // unfortunately, since elle already provides an event loop, the
      // following uses FUSE low-level functions in order to integrate it
      // into the elle event loop.
      //
      {
        int             multithreaded;

        // setup fuse.
        if ((this->fuse = ::fuse_setup(
               (sizeof (arguments) / sizeof (elle::Character*)),
               const_cast<char**>(arguments),
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
        int             fd;

        // retrieve the file descriptor.
        fd = ::fuse_chan_fd(this->channel);

        // allocate the FUSE event broker.
        this->broker =
          new elle::Broker(fd);

        // subscribe to the broker's signal.
        if (this->broker->signal.ready.Subscribe(
              elle::Callback<>::Infer(&InterlacedFUker::Event,
                                      this)) == elle::StatusError)
          escape("unable to subscribe to the signal");

        // activate the broker.
        if (this->broker->Activate() == elle::StatusError)
          escape("unable to activate the broker");

        // attach the broker to the program's event loop.
        if (elle::Program::Attach(*this->broker) == elle::StatusError)
          escape("unable to attach the broker to the event loop");
      }

      return elle::StatusOk;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever data is available on the
    /// FUSE socket.
    ///
    elle::Status        InterlacedFUker::Event(elle::Natural16)
    {
      struct ::fuse_chan*       channel = this->channel;
      char*                     buffer;
      int                       res;

      // allocate a buffer.
      if ((buffer = static_cast<char*>(::malloc(this->size))) == NULL)
        escape("unable to allocate a FUSE buffer");

      std::unique_ptr<char> guard(buffer);

      // retrieve the upcall from the kernel through the FUSE channel.
      res = ::fuse_chan_recv(&channel,
                             buffer,
                             this->size);

      // retry later if necessary.
      if (res == -EINTR)
        return elle::StatusOk;

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

      return elle::StatusOk;
    }

  }
}
