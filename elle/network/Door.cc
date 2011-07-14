//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Door.cc
//
// created       julien quintard   [sat feb  6 04:30:24 2010]
// updated       julien quintard   [thu jul 14 14:07:33 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Door.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Network.hh>

#include <elle/Manifest.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this value defines the time to wait for a door to connect to
    /// a lane after which the connection is assumed to have failed.
    ///
    /// this value is set by default to 1 second.
    ///
    const Natural32		Door::Timeout = 1000;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Door::Door():
      Channel::Channel(Socket::TypeDoor),

      socket(NULL)
    {
    }

    ///
    /// the destructor releases the socket.
    ///
    Door::~Door()
    {
      // check the socket presence.
      if (this->socket != NULL)
	this->socket->deleteLater();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a new door by allocating and setting up a new
    /// socket.
    ///
    /// note that this door is not attached to any lane.
    ///
    Status		Door::Create()
    {
      ::QLocalSocket*	socket;

      // note that the following socket is not tracked for automatic
      // deletion because the next Create() method should take care of it.
      enter();

      // allocate a new socket.
      socket = new ::QLocalSocket;

      // create the door.
      if (this->Create(socket) == StatusError)
	escape("unable to create the door");

      leave();
    }

    ///
    /// this method creates a door based on the given socket.
    ///
    Status		Door::Create(::QLocalSocket*		socket)
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
	    SIGNAL(error(const QLocalSocket::LocalSocketError)),
	    this,
	    SLOT(_error(const QLocalSocket::LocalSocketError))) == false)
	escape("unable to connect to signal");

      leave();
    }

    ///
    /// this method connects the door i.e attaches the socket to a specific
    /// lane.
    ///
    Status		Door::Connect(const String&		name)
    {
      enter();

      /// XXX \todo ca segfault si le client est lance sans serveur...???

      // connect the socket to the server.
      this->socket->connectToServer(name.c_str());

      // wait for the socket to connect.
      if (this->socket->waitForConnected(Door::Timeout) == false)
	escape(this->socket->errorString().toStdString().c_str());

      leave();
    }

    ///
    /// this method disconnects the socket.
    ///
    Status		Door::Disconnect()
    {
      enter();

      // disconnect the socket from the server.
      this->socket->disconnectFromServer();

      leave();
    }

    ///
    /// this method writes a packet to the socket.
    ///
    Status		Door::Write(const Packet&		packet)
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
    Status		Door::Read()
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
    Status		Door::Fetch()
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
    /// this method dumps the door state.
    ///
    Status		Door::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Door]" << std::endl;

      // dump the state.
      std::cout << alignment << Dumpable::Shift << "[Valid] "
		<< this->socket->isValid() << std::endl;

      // dump the full socket path name.
      std::cout << alignment << Dumpable::Shift << "[Path] "
		<< this->socket->fullServerName().toStdString() << std::endl;

      // dump the peer name.
      std::cout << alignment << Dumpable::Shift << "[Peer] "
		<< this->socket->serverName().toStdString() << std::endl;

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever an error occurs.
    ///
    Status		Door::Error(const String&		text)
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
    Status		Door::Dispatch()
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
    /// note here that the type QLocalSocket::LocalSocketError cannot be
    /// written completely ::QLocalSocket::LocalSocketError because the
    /// QT parser is incapable of recognising the type.
    ///
    void		Door::_error(const QLocalSocket::LocalSocketError)
    {
      String		text(this->socket->errorString().toStdString());
      Callback< Parameters<const String> >	callback(&Door::Error, this);
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
    void		Door::_ready()
    {
      Callback< Parameters<> >	callback(&Door::Dispatch, this);
      Closure< Parameters<> >	closure(callback);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert(_(), "unable to spawn a fiber");

      release();
    }

  }
}
