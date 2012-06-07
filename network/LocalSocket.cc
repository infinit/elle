
#include <elle/utility/Buffer.hh>

#include <elle/network/Header.hh>
#include <elle/network/LocalSocket.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Network.hh>

#include <elle/standalone/Morgue.hh>

#include <elle/Manifest.hh>

namespace elle
{
  using namespace standalone;

  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this value defines the time to wait for a socket to connect to
    /// a local server after which the connection is assumed to have failed.
    ///
    /// this value is set by default to 1 second.
    ///
    const Natural32             LocalSocket::Timeout = 1000;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    LocalSocket::LocalSocket():
      AbstractSocket::AbstractSocket(Socket::TypeLocal),

      socket(NULL)
    {
    }

    ///
    /// the destructor releases the socket.
    ///
    LocalSocket::~LocalSocket()
    {
      // check the socket presence.
      if (this->socket != NULL)
        this->socket->deleteLater();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a new socket by allocating and setting up a new
    /// socket.
    ///
    /// note that this socket is not attached to any local server.
    ///
    Status              LocalSocket::Create()
    {
      // allocate a new socket.
      this->socket = new ::QLocalSocket;

      // subscribe to the signal.
      if (this->signal.ready.Subscribe(
            Callback<>::Infer(&LocalSocket::Dispatch, this)) == Status::Error)
        escape("unable to subscribe to the signal");

      // connect the QT signals.
      if (this->connect(this->socket, SIGNAL(connected()),
                        this, SLOT(_connected())) == false)
        escape("unable to connect the signal");

      if (this->connect(this->socket, SIGNAL(disconnected()),
                        this, SLOT(_disconnected())) == false)
        escape("unable to connect the signal");

      if (this->connect(this->socket, SIGNAL(readyRead()),
                        this, SLOT(_ready())) == false)
        escape("unable to connect the signal");

      if (this->connect(
            this->socket,
            SIGNAL(error(const QLocalSocket::LocalSocketError)),
            this,
            SLOT(_error(const QLocalSocket::LocalSocketError))) == false)
        escape("unable to connect to signal");

      return Status::Ok;
    }

    ///
    /// this method creates a socket based on the given socket.
    ///
    Status              LocalSocket::Create(::QLocalSocket*     socket)
    {
      // set the socket.
      this->socket = socket;

      // subscribe to the signal.
      if (this->signal.ready.Subscribe(
            Callback<>::Infer(&LocalSocket::Dispatch, this)) == Status::Error)
        escape("unable to subscribe to the signal");

      // connect the QT signals.
      if (this->connect(this->socket, SIGNAL(connected()),
                        this, SLOT(_connected())) == false)
        escape("unable to connect the signal");

      if (this->connect(this->socket, SIGNAL(disconnected()),
                        this, SLOT(_disconnected())) == false)
        escape("unable to connect the signal");

      if (this->connect(this->socket, SIGNAL(readyRead()),
                        this, SLOT(_ready())) == false)
        escape("unable to connect the signal");

      if (this->connect(
            this->socket,
            SIGNAL(error(const QLocalSocket::LocalSocketError)),
            this,
            SLOT(_error(const QLocalSocket::LocalSocketError))) == false)
        escape("unable to connect to signal");

      // update the state.
      this->state = AbstractSocket::StateConnected;

      return Status::Ok;
    }

    ///
    /// this method connects the socket i.e attaches the socket to a specific
    /// local server.
    ///
    Status              LocalSocket::Connect(
                          const String&                         name,
                          Socket::Mode                          mode)
    {
      // set the state.
      this->state = AbstractSocket::StateConnecting;

      // connect the socket to the server.
      this->socket->connectToServer(name.c_str());

      // depending on the mode.
      switch (mode)
        {
        case Socket::ModeAsynchronous:
          {
            // allocate a timer.
            this->timer = new Timer;

            // create a timer.
            if (this->timer->Create(Timer::ModeSingle) == Status::Error)
              escape("unable to create the callback");

            // subscribe to the timer's signal.
            if (this->timer->signal.timeout.Subscribe(
                  Callback<>::Infer(&LocalSocket::Abort, this)) == Status::Error)
              escape("unable to subscribe to the signal");

            // start the timer.
            if (this->timer->Start(LocalSocket::Timeout) == Status::Error)
              escape("unable to start the timer");

            break;
          }
        case Socket::ModeSynchronous:
          {
            // deliberately wait for the connection to terminate.
            if (this->socket->waitForConnected(LocalSocket::Timeout) == false)
              escape("%s", this->socket->errorString().toStdString().c_str());

            break;
          }
        }

      return Status::Ok;
    }

    ///
    /// this method disconnects the socket.
    ///
    Status              LocalSocket::Disconnect()
    {
      // disconnect the socket from the server.
      this->socket->disconnectFromServer();

      return Status::Ok;
    }

    ///
    /// this method writes a packet to the socket.
    ///
    Status              LocalSocket::Write(const Packet&        packet)
    {
      // check that the socket is connected.
      if (this->state != AbstractSocket::StateConnected)
        escape("the socket does not seem to have been connected");

      // check the size of the packet to make sure the receiver will
      // have a buffer large enough to read it.
      if (packet.Size() > AbstractSocket::Capacity)
        escape("the packet seems to be too large %qu bytes",
               static_cast<Natural64>(packet.Size()));

      // push the packet to the socket.
      if (this->socket->write(
            reinterpret_cast<const char*>(packet.Contents()),
            packet.Size()) != static_cast<qint64>(packet.Size()))
        escape("unable to write the packet");

      // flush to start sending data immediately.
      this->socket->flush();

      return Status::Ok;
    }

    ///
    /// this method reads data from the socket and places it in a buffer.
    ///
    Status              LocalSocket::Read()
    {
      // check that the socket is connected.
      if (this->state != AbstractSocket::StateConnected)
        escape("the socket does not seem to have been connected");

      //
      // read the pending datagrams in the buffer.
      //
      {
        Natural32       size;

        // retrieve the size of the data available.
        size = this->socket->bytesAvailable();

        // check if there is data to be read.
        if (size == 0)
          return Status::Ok;

        // adjust the buffer.
        if (this->buffer == NULL)
          {
            // assign the raw since there was no previous buffer.
            this->buffer = new Region;

            // prepare the capacity.
            if (this->buffer->Prepare(size) == Status::Error)
              escape("unable to prepare the buffer");
          }
        else
          {
            // adjust the buffer.
            if (this->buffer->Adjust(this->buffer->size + size) == Status::Error)
              escape("unable to adjust the buffer");
          }

        // read the packet from the socket.
        if (this->socket->read(
              reinterpret_cast<char*>(this->buffer->contents +
                                      this->buffer->size),
              size) != size)
          escape("%s", this->socket->errorString().toStdString().c_str());

        // set the new size.
        this->buffer->size = this->buffer->size + size;
      }

      return Status::Ok;
    }

    ///
    /// this method extracts as much parcels as possible from the
    /// buffer.
    ///
    Status              LocalSocket::Fetch()
    {
      //
      // try to extract a serie of packet from the received raw.
      //
      while ((this->buffer->size - this->offset) > 0)
        {

          elle::utility::WeakBuffer packet(
              this->buffer->contents + this->offset,
              this->buffer->size - this->offset
          );

          auto reader = packet.Reader();

          auto parcel = std::unique_ptr<Parcel>(new Parcel);
          reader >> *(parcel->header);

          // act depending on the amount of data required against
          // the amount of data received.
          if (reader.Stream().BytesLeft() < parcel->header->size)
            {
              //
              // in this case, the future packet requires more data than
              // has been sent.
              //

              // test if we exceeded the buffer capacity meaning that the
              // waiting packet will probably never come. therefore just
              // discard everything!
              if ((this->buffer->size - this->offset) >
                  AbstractSocket::Capacity)
                goto _disconnect;

              // exit the loop since there is not enough data anyway.
              break;
            }
          else
            {
              //
              // otherwise, there is enough data in the buffer to extract
              // the parcel.
              //

              reader >> *(parcel->data);

              // create the session.
              if (parcel->session->Create(
                    this,
                    Locus::Null,
                    parcel->header->event) == Status::Error)
                escape("unable to create the session");

              // add the parcel to the container.
              this->queue.push_back(parcel.get());

              // stop tracking the parcel.
              parcel.release();

              // move to the next frame by setting the offset at the end of
              // the extracted frame.
              this->offset += reader.Stream().Offset();
            }
        }

      //
      // perform some operations on the buffer.
      //
      {
        // if there is no more data in the buffer, delete it in order to avoid
        // copying data whenever a new packet is received. indeed, if there
        // is no buffer, the packet becomes the buffer, hence simplifying the
        // process.
        if (this->offset == this->buffer->size)
          {
            // delete the buffer.
            delete this->buffer;

            // reinitialize the buffer to NULL.
            this->buffer = NULL;
            this->offset = 0;
          }

        // if the offset is too far, move the existing data to the
        // beginning of the buffer.
        if (this->offset >= AbstractSocket::Capacity)
          {
            // move the data.
            ::memmove(this->buffer->contents,
                      this->buffer->contents + this->offset,
                      this->buffer->size - this->offset);

            // reinitialize the buffer size.
            this->buffer->size = this->buffer->size - this->offset;

            // reinitialize the offset.
            this->offset = 0;
          }
      }

      return Status::Ok;

    _disconnect:
      // purge the errors message.
      purge();

      // disconnect the socket.
      this->Disconnect();

      return Status::Ok;
    }

    ///
    /// this method returns the name the socket is connected to.
    ///
    Status              LocalSocket::Target(String&             name) const
    {
      // check that the socket is connected.
      if (this->state != AbstractSocket::StateConnected)
        escape("the socket does not seem to have been connected");

      // retrieve the server name.
      name = this->socket->serverName().toStdString();

      return Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the socket state.
    ///
    Status              LocalSocket::Dump(const Natural32       margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[LocalSocket]" << std::endl;

      // dump the abstract socket.
      if (AbstractSocket::Dump(margin + 2) == Status::Error)
        escape("unable to dump the abstract socket");

      // dump the state.
      std::cout << alignment << Dumpable::Shift << "[Valid] "
                << this->socket->isValid() << std::endl;

      // dump the full socket path name.
      std::cout << alignment << Dumpable::Shift << "[Path] "
                << this->socket->fullServerName().toStdString() << std::endl;

      // dump the peer name.
      std::cout << alignment << Dumpable::Shift << "[Peer] "
                << this->socket->serverName().toStdString() << std::endl;

      return Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback fetches parcels and dispatches them.
    ///
    Status              LocalSocket::Dispatch()
    {
      // first read from the socket.
      if (this->Read() == Status::Error)
        escape("unable to read from the socket");

      // then, fetch the parcels from the buffer.
      if (this->Fetch() == Status::Error)
        escape("unable to fetch the parcels from the buffer");

      // process the queued parcels.
      while (this->queue.empty() == false)
        {
          // finally, take the oldest parcel and return it.
          std::shared_ptr<Parcel> parcel(this->queue.front());

          // remove this packet.
          this->queue.pop_front();

          // trigger the network shipment mechanism.
          if (Socket::Ship(parcel) == Status::Error)
            log("an error occured while shipping the parcel");
        }

      return Status::Ok;
    }

    ///
    /// this callback is triggered when the channel's timer timeouts i.e
    /// the socket failed to connect within a timeframe.
    ///
    Status              LocalSocket::Abort()
    {
      // bury the timer i.e the system is in the given timer.
      bury(this->timer);

      // reset the timer.
      this->timer = NULL;

      // if the socket has not been connected yet, abort the process.
      if (this->state != AbstractSocket::StateConnected)
        {
          // disconnect the socket.
          if (this->Disconnect() == Status::Error)
            escape("unable to disconnect the socket");
        }

      return Status::Ok;
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered when the socket is considered connected.
    ///
    void                LocalSocket::_connected()
    {
      // set the state.
      this->state = AbstractSocket::StateConnected;

      new reactor::Thread(concurrency::scheduler(), "LocalSocket connected",
                          boost::bind(&Signal<Parameters<> >::Emit,
                                      &this->signal.connected), true);
    }

    ///
    /// this slot is triggered when the socket is considered disconnected
    ///
    void                LocalSocket::_disconnected()
    {
      // set the state.
      this->state = AbstractSocket::StateDisconnected;

      new reactor::Thread(concurrency::scheduler(), "LocalSocket disconnect",
                          boost::bind(&Signal<Parameters<> >::Emit,
                                      &this->signal.disconnected), true);
    }

    ///
    /// this slot is triggered when data is ready on the socket.
    ///
    void                LocalSocket::_ready()
    {
      new reactor::Thread(concurrency::scheduler(), "LocalSocket ready",
                          boost::bind(&Signal<Parameters<> >::Emit,
                                      &this->signal.ready), true);
    }

    ///
    /// this slot is triggered whenever an error occurs.
    ///
    /// note here that the type QLocalSocket::LocalSocketError cannot be
    /// written completely ::QLocalSocket::LocalSocketError because the
    /// QT parser is incapable of recognising the type.
    ///
    void                LocalSocket::_error(
                          const QLocalSocket::LocalSocketError)
    {
      String            cause(this->socket->errorString().toStdString());

      new reactor::Thread
        (concurrency::scheduler(), "LocalSocket error",
         boost::bind(&Signal<Parameters<String> >::Emit,
                     &this->signal.error, cause), true);
    }

  }
}
