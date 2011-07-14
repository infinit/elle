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
// updated       julien quintard   [thu jul 14 14:07:13 2011]
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
    /// this value is set by default to 1 second.
    ///
    const Natural32		Gate::Timeout = 1000;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Gate::Gate():
      Channel::Channel(Socket::TypeGate),

      socket(NULL),
      port(0)
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
      ::QTcpSocket*	socket;

      // note that the following socket is not tracked for automatic
      // deletion because the next Create() method should take care of it.
      enter();

      // allocate a new socket.
      socket = new ::QTcpSocket;

      // create the gate.
      if (this->Create(socket) == StatusError)
	escape("unable to create the gate");

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

      // connect the signals, depending on the mode.
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
    /// this method connects the gate.
    ///
    Status		Gate::Connect(const Address&		address)
    {
      enter();

      // connect the socket to the server.
      this->socket->connectToHost(address.host.location, address.port);

      // wait for the socket to connect.
      if (this->socket->waitForConnected(Gate::Timeout) == false)
	escape(this->socket->errorString().toStdString().c_str());

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
	      Address		address;

	      // extract the data.
	      if (packet.Extract(*parcel->data) == StatusError)
		escape("unable to extract the data");

	      // set the address as being an IP address.
	      if (address.host.Create(Host::TypeIP) == StatusError)
		escape("unable to create an IP address");

	      // create the session.
	      if (parcel->session->Create(
		    this,
		    address,
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

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the gate state.
    ///
    Status		Gate::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Gate]" << std::endl;

      // XXX

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever an error occurs.
    ///
    Status		Gate::Error(const String&		text)
    {
      enter();

      // only process the error if a monitor callback has been registered.
      if (this->callback != NULL)
	{
	  // trigger the callback.
	  if (this->callback->Trigger(text) == StatusError)
	    escape("an error occured in the callback");
	}

      leave();
    }

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
	      Report*	report;

	      // retrieve the report.
	      if (Report::Instance(report) == StatusFalse)
		escape("unable to retrieve the report");

	      // since an error occured, transmit it to the sender
	      if (this->Send(Inputs<TagError>(*report)) == StatusError)
		escape("unable to send an error report");

	      // flush the report since it has been sent to the sender.
	      report->Flush();

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

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered whenever an error occurs.
    ///
    /// note here that the type QAbstractSocket::SocketError cannot be
    /// written completely ::QAbstractSocket::SocketError because the
    /// QT parser is incapable of recognising the type.
    ///
    void		Gate::_error(const QAbstractSocket::SocketError)
    {
      String		text(this->socket->errorString().toStdString());
      Callback< Parameters<const String> >	callback(&Gate::Error, this);
      Closure< Parameters<const String> >	closure(callback, text);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert(_(), "unable to spawn a fiber");

      release();
    }

    ///
    /// this slot fetches and dispatches packets from the socket.
    ///
    void		Gate::_ready()
    {
      Callback< Parameters<> >	callback(&Gate::Dispatch, this);
      Closure< Parameters<> >	closure(callback);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert(_(), "unable to spawn a fiber");

      release();
    }

  }
}
