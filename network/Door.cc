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
// updated       julien quintard   [sat jul  9 16:31:26 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Door.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Raw.hh>
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

    ///
    /// this value defines the maximum capacity of a buffered packed, in bytes.
    ///
    const Natural64		Door::Capacity = 524288;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Door::Door():
      Channel::Channel(Socket::TypeDoor, Socket::ModeUnknown),

      socket(NULL),
      buffer(NULL),
      offset(0)
    {
    }

    ///
    /// specific mode constructor.
    ///
    Door::Door(const Socket::Mode				mode):
      Channel::Channel(Socket::TypeDoor, mode),

      socket(NULL),
      buffer(NULL),
      offset(0)
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

      // delete the buffer.
      if (this->buffer != NULL)
	delete this->buffer;
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
    /// thus this door is already attached to a lane.
    ///
    Status		Door::Create(::QLocalSocket*		socket)
    {
      enter();

      // set the socket.
      this->socket = socket;

      // connect the signals, depending on the mode.
      switch (this->mode)
	{
	case Socket::ModeAsynchronous:
	  {
	    if (this->connect(this->socket, SIGNAL(readyRead()),
			      this, SLOT(_fetch())) == false)
	      escape("unable to connect the signal");

	    if (this->connect(
		  this->socket,
		  SIGNAL(error(const QLocalSocket::LocalSocketError)),
		  this,
		  SLOT(_error(const QLocalSocket::LocalSocketError))) == false)
	      escape("unable to connect to signal");

	    break;
	  }
	case Socket::ModeSynchronous:
	  {
	    // nothing to do.

	    break;
	  }
	case Socket::ModeUnknown:
	default:
	  {
	    escape("unknown mode");
	  }
	}

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
    /// this method reads data from the socket and returns true if a parcel
    /// has been constructed or false if not enough data has been received
    /// to complete a parcel.
    ///
    /// note that since doors are stream-based socket, the data fetched
    /// may be incomplete. in such a case, the data should be stored in
    /// a buffer, waiting for the completing data.
    ///
    /// note however that in order to prevent clients from sending huge
    /// meaningless data the size of a meaningfull packet is limited.
    ///
    Status		Door::Read(Parcel*&			parcel)
    {
      Address		address;

      enter();

      //
      // read the pending datagrams in a raw.
      //
      {
	Raw*		raw;
	Natural32	size;

	enter(instance(raw));

	// retrieve the size of the data available.
	size = this->socket->bytesAvailable();

	// check if there is data to be read.
	if (size == 0)
	  {
	    // if there are bufferised packets, return one.
	    if (this->queue.empty() == false)
	      {
		// finally, take the oldest parcel and return it.
		parcel = this->queue.front();

		// remove this packet.
		this->queue.pop_front();

		true();
	      }
	    else
	      {
		// otherwise, there is no pending parcel at the moment.

		false();
	      }
	  }

	// allocate a new raw.
	raw = new Raw;

	// prepare the raw
	if (raw->Prepare(size) == StatusError)
	  escape("unable to prepare the raw");

	// set the address as being an IP address.
	if (address.host.Create(Host::TypeIP) == StatusError)
	  escape("unable to create an IP address");

	// read the packet from the socket.
	if (this->socket->read((char*)raw->contents, size) != size)
	  escape(this->socket->errorString().toStdString().c_str());

	// set the raw's size.
	raw->size = size;

	// append those raw data to the buffer.
	if (this->buffer == NULL)
	  {
	    // assign the raw since there was no previous buffer.
	    this->buffer = raw;

	    // initialize the offset.
	    this->offset = 0;

	    // stop tracking raw.
	    waive(raw);
	  }
	else
	  {
	    // if the offset is too far, first move the existing data to the
	    // beginning of the buffer.
	    if (this->offset >= Door::Capacity)
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

	    // then append the new data.
	    if (this->buffer->Append(raw->contents, raw->size) == StatusError)
	      escape("unable to append the fetched raw to the buffer");

	    // delete the raw.
	    delete raw;

	    // stop tracking the raw.
	    waive(raw);
	  }

	// release objects.
	release();
      }

      //
      // try to extract a serie of packet from the received raw.
      //
      while ((this->buffer->size - this->offset) > 0)
	{
	  Parcel*	parcel;
	  Region	frame;
	  Packet	packet;

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

	  // test if there is enough data.
	  if ((packet.size - packet.offset) < parcel->header->size)
	    {
	      // test if we exceeded the buffer capacity meaning that the
	      // waiting packet will probably never come. therefore just
	      // discard everything!
	      if ((this->buffer->size - this->offset) > Door::Capacity)
		{
		  // delete the buffer.
		  delete this->buffer;

		  // re-set it to NULL.
		  this->buffer = NULL;
		  this->offset = 0;

		  escape("exceeded the buffer capacity without making sense "
			 "out of the fetched data");
		}

	      // if there are bufferised packets, return one.
	      if (this->queue.empty() == false)
		{
		  // finally, take the oldest parcel and return it.
		  parcel = this->queue.front();

		  // remove this packet.
		  this->queue.pop_front();

		  true();
		}

	      false();
	    }

	  // extract the data.
	  if (packet.Extract(*parcel->data) == StatusError)
	    escape("unable to extract the data");

	  // create the session.
	  if (parcel->session->Create(this,
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

	  // release objects.
	  release();
	}

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
	}

      // finally, take the oldest parcel and return it.
      parcel = this->queue.front();

      // remove this packet.
      this->queue.pop_front();

      true();
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
    /// this callback fetches packets from the socket.
    ///
    Status		Door::Fetch()
    {
      Parcel*		parcel;

      enter(instance(parcel));

      // while there is packets on the socket, read them,
      while (this->Read(parcel) == StatusTrue)
	{
	  // dispatch this parcel to the network manager.
	  //
	  // note that a this point, the network is longer responsible for the
	  // parcel and its memory.
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
    /// this slot fetches packets from the socket.
    ///
    void		Door::_fetch()
    {
      Callback< Parameters<> >	callback(&Door::Fetch, this);
      Closure< Parameters<> >	closure(callback);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert(_(), "unable to spawn a fiber");

      release();
    }

  }
}
