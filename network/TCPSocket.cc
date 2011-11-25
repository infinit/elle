//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed may 25 11:01:56 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/TCPSocket.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Network.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this value defines the time to wait for a TCP socket to connect to
    /// a TCP server after which the connection is assumed to have failed.
    ///
    /// this value is set by default to 5 seconds.
    ///
    const Natural32		TCPSocket::Timeout = 5000;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    TCPSocket::TCPSocket():
      StreamSocket::StreamSocket(Socket::TypeTCP),

      socket(NULL)
    {
    }

    ///
    /// the destructor releases the socket.
    ///
    TCPSocket::~TCPSocket()
    {
      // check the socket presence.
      if (this->socket != NULL)
	this->socket->deleteLater();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a new TCP socket by allocating and setting up a new
    /// socket.
    ///
    /// note that at this stage, the TCP socket is not attached to a
    /// TCP server.
    ///
    Status		TCPSocket::Create()
    {
      enter();

      // allocate a new socket.
      this->socket = new ::QTcpSocket;

      // subscribe to the signal.
      if (this->signal.ready.Subscribe(
	    Callback<>::Infer(&TCPSocket::Dispatch, this)) == StatusError)
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
	    SIGNAL(error(const QAbstractSocket::SocketError)),
	    this,
	    SLOT(_error(const QAbstractSocket::SocketError))) == false)
	escape("unable to connect to signal");

      leave();
    }

    ///
    /// this method creates a TCP socket based on the given QT socket.
    ///
    Status		TCPSocket::Create(::QTcpSocket*		socket)
    {
      enter();

      // set the socket.
      this->socket = socket;

      // subscribe to the signal.
      if (this->signal.ready.Subscribe(
	    Callback<>::Infer(&TCPSocket::Dispatch, this)) == StatusError)
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
	    SIGNAL(error(const QAbstractSocket::SocketError)),
	    this,
	    SLOT(_error(const QAbstractSocket::SocketError))) == false)
	escape("unable to connect to signal");

      // set the socket as being connected.
      this->state = StreamSocket::StateConnected;

      leave();
    }

    ///
    /// this method connects the socket.
    ///
    Status		TCPSocket::Connect(
                          const Locus&				locus,
			  const Socket::Mode			mode)
    {
      enter();

      // update the state.
      this->state = StreamSocket::StateConnecting;

      // connect the socket to the server.
      this->socket->connectToHost(locus.host.location, locus.port);

      // depending on the mode.
      switch (mode)
	{
	case Socket::ModeAsynchronous:
	  {
	    // allocate a timer.
	    this->timer = new Timer;

	    // create a timer.
	    if (this->timer->Create(Timer::ModeSingle) == StatusError)
	      escape("unable to create the callback");

	    // subscribe to the timer's signal.
	    if (this->timer->signal.timeout.Subscribe(
		  Callback<>::Infer(&TCPSocket::Abort, this)) == StatusError)
	      escape("unable to subscribe to the signal");

	    // start the timer.
	    if (this->timer->Start(TCPSocket::Timeout) == StatusError)
	      escape("unable to start the timer");

	    break;
	  }
	case Socket::ModeSynchronous:
	  {
	    // deliberately wait for the connection to terminate.
	    if (this->socket->waitForConnected(TCPSocket::Timeout) == false)
	      escape(this->socket->errorString().toStdString().c_str());

	    break;
	  }
	}

      leave();
    }

    ///
    /// this method disconnects the socket.
    ///
    Status		TCPSocket::Disconnect()
    {
      enter();

      // disconnect the socket from the server.
      this->socket->disconnectFromHost();

      leave();
    }

    ///
    /// this method writes the given packet to the socket.
    ///
    Status		TCPSocket::Write(const Packet&		packet)
    {
      enter();

      // check that the socket is connected.
      if (this->state != StreamSocket::StateConnected)
	escape("the socket does not seem to have been connected");

      // check the size of the packet to make sure the receiver will
      // have a buffer large enough to read it.
      if (packet.size > StreamSocket::Capacity)
	escape("the packet seems to be too large: %qu bytes",
	       static_cast<Natural64>(packet.size));

      // push the packet to the socket.
      if (this->socket->write(
	    reinterpret_cast<const char*>(packet.contents),
	    packet.size) != static_cast<qint64>(packet.size))
	escape("unable to write the packet");

      // flush to start sending data immediately.
      this->socket->flush();

      leave();
    }

    ///
    /// this method reads data from the socket and places it in a buffer.
    ///
    Status		TCPSocket::Read()
    {
      enter();

      // check that the socket is connected.
      if (this->state != StreamSocket::StateConnected)
	escape("the socket does not seem to have been connected");

      //
      // read the pending datagrams in the buffer.
      //
      {
	Natural32	size;

	// retrieve the size of the data available.
	size = this->socket->bytesAvailable();

	// check if there is data to be read.
	if (size == 0)
	  leave();

	// adjust the buffer.
	if (this->buffer == NULL)
	  {
	    // assign the raw since there was no previous buffer.
	    this->buffer = new Region;

	    // prepare the capacity.
	    if (this->buffer->Prepare(size) == StatusError)
	      escape("unable to prepare the buffer");
	  }
	else
	  {
	    // adjust the buffer.
	    if (this->buffer->Adjust(this->buffer->size + size) == StatusError)
	      escape("unable to adjust the buffer");
	  }

	// read the packet from the socket.
	if (this->socket->read(
	      reinterpret_cast<char*>(this->buffer->contents +
				      this->buffer->size),
	      size) != size)
	  escape(this->socket->errorString().toStdString().c_str());

	// set the new size.
	this->buffer->size = this->buffer->size + size;
      }

      leave();
    }

    ///
    /// this method extracts as much parcels as possible from the
    /// buffer.
    ///
    Status		TCPSocket::Fetch()
    {
      enter();

      //
      // try to extract a serie of packet from the received raw.
      //
      while ((this->buffer->size - this->offset) > 0)
	{
	  Packet	packet;
	  Region	frame;
	  Parcel*	parcel;

	  enterx(instance(parcel));

	  // create the frame based on the previously extracted raw.
	  if (frame.Wrap(this->buffer->contents + this->offset,
			 this->buffer->size - this->offset) == StatusError)
	    escape("unable to wrap a frame in the raw");

	  // prepare the packet based on the frame.
	  if (packet.Wrap(frame) == StatusError)
	    escape("unable to prepare the packet");

	  // allocate the parcel.
	  parcel = new Parcel;

	  // extract the header.
	  if (parcel->header->Extract(packet) == StatusError)
	    escape("unable to extract the header");

	  // act depending on the amount of data required against
	  // the amount of data received.
	  if ((packet.size - packet.offset) < parcel->header->size)
	    {
	      //
	      // in this case, the future packet requires more data than
	      // has been sent.
	      //

	      // test if we exceeded the buffer capacity meaning that the
	      // waiting packet will probably never come. therefore just
	      // discard everything!
	      if ((this->buffer->size - this->offset) >
		  StreamSocket::Capacity)
		{
		  // delete the buffer.
		  delete this->buffer;

		  // re-set it to NULL.
		  this->buffer = NULL;
		  this->offset = 0;

		  // log the event.
		  log("exceeded the buffer capacity without making sense "
		      "out of the fetched data");
		}

	      // since the parcel will not be built, delete the instance.
	      delete parcel;

	      // waive tracking.
	      waive(parcel);

	      // exit the loop since there is not enough data anyway.
	      break;
	    }
	  else
	    {
	      //
	      // otherwise, there is enough data in the buffer to extract
	      // the parcel.
	      //
	      Locus		locus;

	      // extract the data.
	      if (packet.Extract(*parcel->data) == StatusError)
		escape("unable to extract the data");

	      // retrieve the socket's target.
	      if (this->Target(locus) == StatusError)
		escape("unable to retrieve the source locus");

	      // create the session.
	      if (parcel->session->Create(
		    this,
		    locus,
		    parcel->header->event) == StatusError)
		escape("unable to create the session");

	      // add the parcel to the container.
	      this->queue.push_back(parcel);

	      // stop tracking the parcel.
	      waive(parcel);

	      // move to the next frame by setting the offset at the end of
	      // the extracted frame.
	      this->offset = this->offset + packet.offset;
	    }

	  // release objects.
	  release();
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

	    // reinitialize the locuser to NULL.
	    this->buffer = NULL;
	    this->offset = 0;
	  }

	// if the offset is too far, move the existing data to the
	// beginning of the buffer.
	if (this->offset >= StreamSocket::Capacity)
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

      leave();
    }

    ///
    /// this method returns the locus the socket is connected to.
    ///
    Status		TCPSocket::Target(Locus&		locus) const
    {
      Host		host;
      Port		port;

      enter();

      // check that the socket is connected.
      if (this->state != StreamSocket::StateConnected)
	escape("the socket does not seem to have been connected");

      // create the host.
      if (host.Create(this->socket->peerAddress().toString().toStdString()) ==
	  StatusError)
	escape("unable to create the host");

      // create the port.
      port = this->socket->peerPort();

      // create the locus.
      if (locus.Create(host, port) == StatusError)
	escape("unable to create the locus");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the socket state.
    ///
    Status		TCPSocket::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Locus		locus;

      enter();

      std::cout << alignment << "[TCPSocket]" << std::endl;

      // dump the channel.
      if (StreamSocket::Dump(margin + 2) == StatusError)
	escape("unable to dump the channel");

      // retrieve the target.
      if (this->Target(locus) == StatusError)
	escape("unable to retrieve the target");

      // dump the locus.
      if (locus.Dump(margin + 2) == StatusError)
	escape("unable to dump the locus");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback fetches parcels and dispatches them.
    ///
    Status		TCPSocket::Dispatch()
    {
      enter();

      // first read from the socket.
      if (this->Read() == StatusError)
	escape("unable to read from the socket");

      // then, fetch the parcels from the buffer.
      if (this->Fetch() == StatusError)
	escape("unable to fetch the parcels from the buffer");

      // process the queued parcels.
      while (this->queue.empty() == false)
	{
	  Parcel*	parcel;

	  enterx(instance(parcel));

	  // finally, take the oldest parcel and return it.
	  parcel = this->queue.front();

	  // remove this packet.
	  this->queue.pop_front();

	  // trigger the network shipment mechanism.
	  if (Socket::Ship(parcel) == StatusError)
	    log("an error occured while shipping the parcel");

	  // stop tracking the parcel since Ship() will have taken
	  // care of it.
	  waive(parcel);

	  release();
	}

      leave();
    }

    ///
    /// this callback is triggered when the channel's timer timeouts i.e
    /// the socket failed to connect within a timeframe.
    ///
    Status		TCPSocket::Abort()
    {
      enter();

      // delete the timer.
      delete this->timer;

      // reset the locuser.
      this->timer = NULL;

      // if the socket has not been connected yet, abort the process.
      if (this->state != StreamSocket::StateConnected)
	{
	  // disconnect the socket.
	  if (this->Disconnect() == StatusError)
	    escape("unable to disconnect the socket");
	}

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered when the socket is considered connected.
    ///
    void		TCPSocket::_connected()
    {
      Closure<
	Status,
	Parameters<>
	>		closure(Callback<>::Infer(&Signal<
						    Parameters<>
						    >::Emit,
						  &this->signal.connected));

      enter();

      // set the state.
      this->state = StreamSocket::StateConnected;

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	yield(_(), "unable to spawn a fiber");

      release();
    }

    ///
    /// this slot is triggered when the socket is considered disconnected.
    ///
    void		TCPSocket::_disconnected()
    {
      Closure<
	Status,
	Parameters<>
	>		closure(Callback<>::Infer(&Signal<
						    Parameters<>
						    >::Emit,
						  &this->signal.disconnected));

      enter();

      // set the state.
      this->state = StreamSocket::StateDisconnected;

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	yield(_(), "unable to spawn a fiber");

      release();
    }

    ///
    /// this slot is triggered when data is ready on the socket.
    ///
    void		TCPSocket::_ready()
    {
      Closure<
	Status,
	Parameters<>
	>		closure(Callback<>::Infer(&Signal<
						    Parameters<>
						    >::Emit,
						  &this->signal.ready));

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	yield(_(), "unable to spawn a fiber");

      release();
    }

    ///
    /// this slot is triggered whenever an error occurs.
    ///
    /// note here that the type QAbstractSocket::SocketError cannot be
    /// written completely ::QAbstractSocket::SocketError because the
    /// QT parser is incapable of recognising the type.
    ///
    void		TCPSocket::_error(
                          const QAbstractSocket::SocketError)
    {
      String		cause(this->socket->errorString().toStdString());
      Closure<
	Status,
	Parameters<
	  const String&
	  >
	>		closure(Callback<>::Infer(&Signal<
						    Parameters<
						      const String&
						      >
						    >::Emit,
						  &this->signal.error),
				cause);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	yield(_(), "unable to spawn a fiber");

      release();
    }

  }
}
