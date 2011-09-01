//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Gate.cc
//
// created       julien quintard   [wed may 25 11:01:56 2011]
// updated       julien quintard   [wed aug 31 18:05:37 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Gate.hh>
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
    /// this value defines the time to wait for a gate to connect to
    /// a bridge after which the connection is assumed to have failed.
    ///
    /// this value is set by default to 5 seconds.
    ///
    const Natural32		Gate::Timeout = 5000;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Gate::Gate():
      Channel::Channel(Socket::TypeGate),

      socket(NULL)
    {
    }

    ///
    /// the destructor releases the socket.
    ///
    Gate::~Gate()
    {
      // check the socket presence.
      if (this->socket != NULL)
	this->socket->deleteLater();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a new gate by allocating and setting up a new
    /// socket.
    ///
    /// note that at this stage, the gate is not attached to a bridge.
    ///
    Status		Gate::Create()
    {
      enter();

      // allocate a new socket.
      this->socket = new ::QTcpSocket;

      // connect the signals.
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
    /// this method creates a gate based on the given socket.
    ///
    Status		Gate::Create(::QTcpSocket*		socket)
    {
      enter();

      // set the socket.
      this->socket = socket;

      // connect the signals.
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

      // set the gate as being connected.
      this->state = Channel::StateConnected;

      leave();
    }

    ///
    /// this method connects the gate.
    ///
    Status		Gate::Connect(const Point&		point,
				      const Channel::Mode	mode)
    {
      Callback< Status,
		Parameters<> >	callback(&Gate::Abort, this);

      enter();

      // allocate a timer.
      this->timer = new Timer;

      // create a timer.
      if (this->timer->Create(Timer::ModeSingle, callback) == StatusError)
	escape("unable to create the callback");

      // start the timer.
      if (this->timer->Start(Gate::Timeout) == StatusError)
	escape("unable to start the timer");

      // update the state.
      this->state = Channel::StateConnecting;

      // connect the socket to the server.
      this->socket->connectToHost(point.host.location, point.port);

      // depending on the mode.
      switch (mode)
	{
	case Channel::ModeAsynchronous:
	  {
	    // do nothing and wait for the 'connected' signal.

	    break;
	  }
	case Channel::ModeSynchronous:
	  {
	    // deliberately wait for the connection to terminate.
	    if (this->socket->waitForConnected(Gate::Timeout) == false)
	      escape(this->socket->errorString().toStdString().c_str());

	    break;
	  }
	}

      leave();
    }

    ///
    /// this method disconnects the socket.
    ///
    Status		Gate::Disconnect()
    {
      enter();

      // disconnect the socket from the server.
      this->socket->disconnectFromHost();

      leave();
    }

    ///
    /// this method writes the given packet to the socket.
    ///
    Status		Gate::Write(const Packet&		packet)
    {
      enter();

      // check the size of the packet to make sure the receiver will
      // have a buffer large enough to read it.
      if (packet.size > Channel::Capacity)
	escape("the packet seems to be too large: %qu bytes",
	       packet.size);

      // push the packet to the socket.
      if (this->socket->write((const char*)packet.contents,
			      packet.size) != (::qint64)packet.size)
	escape("unable to write the packet");

      // flush to start sending data immediately.
      this->socket->flush();

      leave();
    }

    ///
    /// this method reads data from the socket and places it in a buffer.
    ///
    Status		Gate::Read()
    {
      enter();

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
	if (this->socket->read((char*)this->buffer->contents +
			       this->buffer->size, size) != size)
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
    Status		Gate::Fetch()
    {
      enter();

      //
      // try to extract a serie of packet from the received raw.
      //
      while ((this->buffer->size - this->offset) > 0)
	{
	  Region	frame;
	  Packet	packet;
	  Parcel*	parcel;

	  enter(instance(parcel));

	  // create the frame based on the previously extracted raw.
	  if (frame.Wrap(this->buffer->contents + this->offset,
			 this->buffer->size - this->offset) == StatusError)
	    escape("unable to wrap a frame in the raw");

	  // prepare the packet based on the frame.
	  if (packet.Prepare(frame) == StatusError)
	    escape("unable to prepare the packet");

	  // detach the frame from the packet so that the region is
	  // not released once the packet is destroyed. indeed, since the
	  // frame is a chunk, it must not be freed.
	  if (packet.Detach() == StatusError)
	    escape("unable to detach the data from the packet");

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
	      if ((this->buffer->size - this->offset) > Channel::Capacity)
		{
		  // delete the buffer.
		  delete this->buffer;

		  // re-set it to NULL.
		  this->buffer = NULL;
		  this->offset = 0;

		  /// XXX \todo make this one a warning instead of an error.
		  escape("exceeded the buffer capacity without making sense "
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
	      Point		point;

	      // extract the data.
	      if (packet.Extract(*parcel->data) == StatusError)
		escape("unable to extract the data");

	      // retrieve the gate's target.
	      if (this->Target(point) == StatusError)
		escape("unable to retrieve the source point");

	      // create the session.
	      if (parcel->session->Create(
		    this,
		    point,
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

	    // reinitialize the pointer to NULL.
	    this->buffer = NULL;
	    this->offset = 0;
	  }

	// if the offset is too far, move the existing data to the
	// beginning of the buffer.
	if (this->offset >= Channel::Capacity)
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
    /// this method returns the point the gate is connected to.
    ///
    Status		Gate::Target(Point&			point) const
    {
      Host		host;
      Port		port;

      enter();

      // check the socket state.
      if (this->state != Channel::StateConnected)
	escape("unable to retrieve the target address of a non-connected "
	       "gate");

      // create the host.
      if (host.Create(this->socket->peerAddress().toString().toStdString()) ==
	  StatusError)
	escape("unable to create the host");

      // create the port.
      port = this->socket->peerPort();

      // create the point.
      if (point.Create(host, port) == StatusError)
	escape("unable to create the point");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the gate state.
    ///
    Status		Gate::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Point		point;

      enter();

      std::cout << alignment << "[Gate]" << std::endl;

      // dump the channel.
      if (Channel::Dump(margin + 2) == StatusError)
	escape("unable to dump the channel");

      // retrieve the target.
      if (this->Target(point) == StatusError)
	escape("unable to retrieve the target");

      // dump the point.
      if (point.Dump(margin + 2) == StatusError)
	escape("unable to dump the point");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback fetches parcels and dispatches them.
    ///
    Status		Gate::Dispatch()
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

	  // finally, take the oldest parcel and return it.
	  parcel = this->queue.front();

	  // remove this packet.
	  this->queue.pop_front();

	  // otherwise, trigger the network dispatching mechanism.
	  if (Network::Dispatch(parcel) == StatusError)
	    {
	      // log the errors.
	      log("an error occured while dispatching a message");

	      // stop tracking the parcel since it should have been deleted
	      // in Dispatch().
	      waive(parcel);

	      leave();
	    }

	  // stop tracking the parcel.
	  waive(parcel);

	  release();
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Gate::Abort()
    {
      enter();

      // delete the timer.
      delete this->timer;

      // reset the pointer.
      this->timer = NULL;

      // if the socket has not been connected yet, abort the process.
      if (this->state != Channel::StateConnected)
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
    /// XXX
    ///
    void		Gate::_connected()
    {
      enter();

      // update the state.
      this->state = Channel::StateConnected;

      // finally, notify the monitoring callback.
      if (this->Signal() == StatusError)
	alert(_(), "unable to signal the change of state");

      release();
    }

    ///
    /// XXX
    ///
    void		Gate::_disconnected()
    {
      enter();

      // update the state.
      this->state = Channel::StateDisconnected;

      // finally, notify the monitoring callback.
      if (this->Signal() == StatusError)
	alert(_(), "unable to signal the change of state");

      release();
    }

    ///
    /// this slot fetches and dispatches packets from the socket.
    ///
    void		Gate::_ready()
    {
      Callback< Status,
		Parameters<> >	callback(&Gate::Dispatch, this);
      Closure< Status,
	       Parameters<> >	closure(callback);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert(_(), "unable to spawn a fiber");

      release();
    }

    ///
    /// this slot is triggered whenever an error occurs.
    ///
    /// note here that the type QAbstractSocket::SocketError cannot be
    /// written completely ::QAbstractSocket::SocketError because the
    /// QT parser is incapable of recognising the type.
    ///
    void		Gate::_error(const QAbstractSocket::SocketError)
    {
      enter();

      switch (this->state)
	{
	case Channel::StateConnecting:
	  {
	    //
	    // if the socket was connecting and an error occured, set
	    // the state as disconnected and signal it.
	    //

	    // update the state.
	    this->state = Channel::StateDisconnected;

	    Callback< Status,
		      Parameters<> >	callback(&Channel::Signal, this);
	    Closure< Status,
		     Parameters<> >	closure(callback);

	    // spawn a fiber.
	    if (Fiber::Spawn(closure) == StatusError)
	      alert(_(), "unable to spawn a fiber");

	    break;
	  }
	case Channel::StateConnected:
	  {
	    //
	    // if the socket is connected and an error occured, close
	    // the socket.
	    //

	    Callback< Status,
		      Parameters<> >	callback(&Gate::Disconnect, this);
	    Closure< Status,
		     Parameters<> >	closure(callback);

	    // spawn a fiber.
	    if (Fiber::Spawn(closure) == StatusError)
	      alert(_(), "unable to spawn a fiber");

	    break;
	  }
	default:
	  {
	    alert(_(), "unexpected socket state '%u'",
		  this->state);
	  }
	}

      release();
    }

  }
}
