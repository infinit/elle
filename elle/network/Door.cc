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
// updated       julien quintard   [wed mar 10 20:51:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Door.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this value defines the time to wait for a door to connect to
    /// a bridge after which the connection is assumed to have failed.
    ///
    /// this value is set by default to 1 second.
    ///
    const Natural32		Door::Timeout = 1000;

    ///
    /// this value defines the maximum capacity of a buffered packed, in bytes.
    ///
    const Natural32		Door::Capacity = 524288;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Door::Door():
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
	delete this->socket;

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

      // connect the signals.
      if (this->connect(this->socket, SIGNAL(disconnected()),
			this, SIGNAL(Disconnected())) == false)
	escape("unable to connect the signal");

      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Fetch())) == false)
	escape("unable to connect the signal");

      if (this->connect(this->socket,
			SIGNAL(error(QLocalSocket::LocalSocketError)),
			this,
			SLOT(Error(QLocalSocket::LocalSocketError))) ==
	  false)
	escape("unable to connect to signal");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Door::Connect(const String&		name)
    {
      enter();

      // connect the socket to the server.
      this->socket->connectToServer(name.c_str());

      // wait for the socket to connect.
      if (this->socket->waitForConnected(Door::Timeout) == false)
	escape(this->socket->errorString().toStdString().c_str());

      leave();
    }

    ///
    /// XXX
    ///
    Status		Door::Disconnect()
    {
      enter();

      // disconnect the socket from the server.
      this->socket->disconnectFromServer();

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Door::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Door]" << std::endl;

      // dump the state.
      std::cout << alignment << shift << "[Valid] "
		<< this->socket->isValid() << std::endl;

      // dump the full socket path name.
      std::cout << alignment << shift << "[Path] "
		<< this->socket->fullServerName().toStdString() << std::endl;

      // dump the peer name.
      std::cout << alignment << shift << "[Peer] "
		<< this->socket->serverName().toStdString() << std::endl;

      leave();
    }

//
// ---------- signals ---------------------------------------------------------
//

    /// \todo XXX disconnected

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
    void		Door::Error(QLocalSocket::LocalSocketError	error)
    {
      enter();

      // basically just report the error without doing anything else.
      alert(this->socket->errorString().toStdString().c_str());
    }

    ///
    /// this method fetches packets from the socket.
    ///
    /// note that since doors are stream-based socket, the data fetched
    /// may be incomplete. in such a case, the data should be stored in
    /// a buffer, waiting for the completing data.
    ///
    /// note however that in order to prevent clients from sending huge
    /// meaningless data in order to force the server to buffer data,
    /// the size of a meaningfull packet is limited.
    ///
    void		Door::Fetch()
    {
      Raw*		raw;
      Address		address;

      enter(instance(raw));

      printf("[XXX] Door::Fetch(%u)\n", this->socket->bytesAvailable());

      //
      // read the pending datagrams in a raw.
      //
      {
	Natural32	size;

	// retrieve the size of the data available.
	size = this->socket->bytesAvailable();

	// check if there is data to be read.
	if (size == 0)
	  return;

	// allocate a new raw.
	raw = new Raw;

	// prepare the raw
	if (raw->Prepare(size) == StatusError)
	  alert("unable to prepare the raw");

	// set the address as being an IP address.
	if (address.host.Create(Host::TypeIP) == StatusError)
	  alert("unable to create an IP address");

	// read the packet from the socket.
	if (this->socket->read((char*)raw->contents, size) != size)
	  alert(this->socket->errorString().toStdString().c_str());

	// set the raw's size.
	raw->size = size;

	// append those raw data to the buffer.
	if (this->buffer == NULL)
	  this->buffer = raw;
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
	      alert("unable to append the fetched raw to the buffer");

	    // delete the raw.
	    delete raw;
	  }

	// stop tracking the raw.
	waive(raw);
      }

      //
      // try to extract a serie of packet from the received raw.
      //
      while ((this->buffer->size - this->offset) > 0)
	{
	  Region	frame;
	  Packet	packet;
	  Context*	context;
	  Header*	header;
	  Data*		data;

	  enter(instance(context),
		instance(header),
		instance(data));

	  // create the frame based on the previously extracted raw.
	  if (frame.Wrap(this->buffer->contents + this->offset,
			 this->buffer->size - this->offset) == StatusError)
	    alert("unable to wrap a frame in the raw");

	  // prepare the packet based on the frame.
	  if (packet.Prepare(frame) == StatusError)
	    alert("unable to prepare the packet");

	  // detach the frame from the packet so that the region is
	  // not released once the packet is destroyed.
	  if (packet.Detach() == StatusError)
	    alert("unable to detach the frame");

	  // allocate the header.
	  header = new Header;

	  // extract the header.
	  if (header->Extract(packet) == StatusError)
	    alert("unable to extract the header");

	  // test if there is enough data.
	  if ((Integer32)((packet.size - packet.offset) - header->size) < 0)
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

		  alert("exceeded the buffer capacity without making sense "
			"out of the fetched data");
		}

	      alert("not enough data to extract the whole packet");
	    }

	  // allocate the data.
	  data = new Data;

	  // extract the data.
	  if (packet.Extract(*data) == StatusError)
	    alert("unable to extract the data");

	  // allocate the context.
	  context = new Context(this, address, header->identifier);

	  // record this packet to the network manager.
	  if (Network::Dispatch(context, header, data) == StatusError)
	    alert("unable to record the packet");

	  // move to the next frame by setting the offset at the end of
	  // the extracted frame.
	  this->offset = this->offset + packet.offset;

	  release();
	}

      release();
    }

  }
}
