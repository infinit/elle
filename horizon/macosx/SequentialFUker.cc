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

#include <horizon/macosx/SequentialFUker.hh>
#include <horizon/macosx/FUSE.hh>

#include <hole/Hole.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    SequentialFUker::SequentialFUker():
      broker(NULL),
      fuse(NULL),
      mountpoint(NULL),
      session(NULL),
      channel(NULL),
      size(0),
      state(SequentialFUker::StateWaiting)
    {
    }

    ///
    /// destructor.
    ///
    SequentialFUker::~SequentialFUker()
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
        SequentialFUker::Scoutor        scoutor;

        // release the buffers.
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            SequentialFUker::Item       item = *scoutor;

            ::free(item.buffer);
          }

        // clear the container.
        this->container.clear();

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
    /// \todo XXX the -o direct_io messes up with the permissions such
    ///   that $> ./configure does not work. direct_io is however
    ///   necessary for etoile to work.
    ///
    elle::Status        SequentialFUker::Setup()
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
      // note that compared to the Linux implementation, the MacOS X
      // FUSE does not support the -o large_read and -o big_writes
      // options.
      //
      elle::String      ofsname("-ofsname=" +
                                hole::Hole::Descriptor.name);
      elle::String      ovolname("-ovolname=" +
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
          "-ofstypename=infinit",

          //
          // this option disables remote file locking.
          //
          "-ono_remote_lock",

          //
          // this option activates the in-kernel caching based on
          // the modification times.
          //
          "-oauto_cache",

          //
          // this options activates the MacOS X ACLs which are set through
          // extended attributes.
          //
          "-oextended_security",

          //
          // this option forbids the access to and creation of .DS_Store files
          // along with any ._* file.
          //
          "-onoappledouble",

          //
          // these options activate the custom volume icon.
          //
          // note that Apple icon (icns) files can be generated
          // with the _makeicns_ binary.
          //
          // XXX[we should put this file as an output of the
          //     compilation/installation process]
          //
          "-omodules=volicon",
          "-oiconpath=../../temporary/nazi.icns",

          //
          // this option specifies the name of the file system instance.
          //
          ofsname.c_str(),

          //
          // this option provides a user-friendly name for the MacOS X volume.
          //
          ovolname.c_str(),

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
                            sizeof (arguments) / sizeof (elle::Character*),
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
        this->broker = new elle::Broker(fd);

        // subscribe to the timer's signal.
        if (this->broker->signal.ready.Subscribe(
              elle::Callback<>::Infer(&SequentialFUker::Event,
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
    elle::Status        SequentialFUker::Event(elle::Natural16)
    {
      struct ::fuse_chan*       channel = this->channel;
      SequentialFUker::Item     item;

      // allocate a buffer.
      if ((item.buffer = static_cast<char*>(::malloc(this->size))) == NULL)
        escape("unable to allocate a FUSE buffer");

      std::unique_ptr<char> guard(item.buffer);

      // retrieve the upcall from the kernel through the FUSE channel.
      item.res = ::fuse_chan_recv(&channel,
                                  item.buffer,
                                  this->size);

      // retry later if necessary.
      if (item.res == -EINTR)
        return elle::StatusOk;

      // exit if an error occured.
      if (item.res <= 0)
        {
          if (elle::Program::Exit() == elle::StatusError)
            escape("unable to exit the program");
        }

      // record the item.
      this->container.push_back(item);

      guard.release();

      // if the FUker is already processing, return since this
      // FUker processes events in a sequential manner.
      if (this->state == SequentialFUker::StateProcessing)
        return elle::StatusOk;

      // set the state.
      this->state = SequentialFUker::StateProcessing;

      // as long as the container contains buffers...
      while (this->container.empty() == false)
        {
          SequentialFUker::Item item;

          // retrieve the next item.
          item = this->container.front();

          // remove it from the container.
          this->container.pop_front();

          // trigger the upcall handler.
          ::fuse_session_process(this->session,
                                 item.buffer,
                                 item.res,
                                 channel);

          // release the buffer.
          ::free(item.buffer);
          item.buffer = nullptr;
        }

      // reset the state.
      this->state = SequentialFUker::StateWaiting;

      return elle::StatusOk;
    }

  }
}
