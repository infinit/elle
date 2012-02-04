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
      elle::String      ofsname("-ofsname='" +
                                hole::Hole::Descriptor.name +
                                "'");
      const char*               arguments[] =
        {
          "horizon",

          "-s",

          "-f",
          "-o", "subtype=infinit",

          "-o", "no_remote_lock",
          // XXX "-o", "large_read",
          // XXX "-o", "big_writes",
          "-o", "auto_cache",
          "-o", "direct_io",

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

      leave();
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

      enterx(slab(item.buffer, ::free));

      // allocate a buffer.
      if ((item.buffer = static_cast<char*>(::malloc(this->size))) == NULL)
        escape("unable to allocate a FUSE buffer");

      // retrieve the upcall from the kernel through the FUSE channel.
      item.res = ::fuse_chan_recv(&channel,
                                  item.buffer,
                                  this->size);

      // retry later if necessary.
      if (item.res == -EINTR)
        leave();

      // exit if an error occured.
      if (item.res <= 0)
        {
          if (elle::Program::Exit() == elle::StatusError)
            escape("unable to exit the program");
        }

      // record the item.
      this->container.push_back(item);

      // waive the tracking.
      waive(item.buffer);

      // if the FUker is already processing, return since this
      // FUker processes events in a sequential manner.
      if (this->state == SequentialFUker::StateProcessing)
        leave();

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
        }

      // reset the state.
      this->state = SequentialFUker::StateWaiting;

      leave();
    }

  }
}
