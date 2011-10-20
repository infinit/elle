//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed feb  3 21:52:30 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Slot.hh>
#include <elle/network/Raw.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Network.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Slot::Slot():
      Socket::Socket(Socket::TypeSlot),

      port(0),
      socket(NULL)
    {
    }

    ///
    /// the destructor releases the associated resources.
    ///
    Slot::~Slot()
    {
      // release the socket.
      if (this->socket != NULL)
	this->socket->deleteLater();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a slot and connects the default signals.
    ///
    Status		Slot::Create()
    {
      enter();

      // allocate a new UDP socket.
      this->socket = new ::QUdpSocket;

      // bind the socket.
      if (this->socket->bind() == false)
	escape(this->socket->errorString().toStdString().c_str());

      // retrieve the port.
      this->port = this->socket->localPort();

      // subscribe to the signal.
      if (this->signal.ready.Subscribe(
	    Callback<>::Infer(&Slot::Dispatch, this)) == StatusError)
	escape("unable to subscribe to the signal");

      // connect the QT signals, depending on the mode.
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
    /// this method creates a slot with a specific port.
    ///
    Status		Slot::Create(const Port			port)
    {
      enter();

      // set the port.
      this->port = port;

      // allocate a new UDP socket.
      this->socket = new ::QUdpSocket;

      // bind the socket to the port.
      if (this->socket->bind(this->port) == false)
	escape(this->socket->errorString().toStdString().c_str());

      // subscribe to the signal.
      if (this->signal.ready.Subscribe(
	    Callback<>::Infer(&Slot::Dispatch, this)) == StatusError)
	escape("unable to subscribe to the signal");

      // connect the QT signals.
      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Fetch())) == false)
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
    /// this method writes a packet on the socket so that it gets sent
    /// to the given locus.
    ///
    Status		Slot::Write(const Locus&		locus,
				    const Packet&		packet)
    {
      enter();

      // check the size of the packet to make sure the receiver will
      // have a buffer large enough to read it.
      if (packet.size > Channel::Capacity)
	escape("the packet seems to be too large: %qu bytes",
	       static_cast<Natural64>(packet.size));

      // push the datagram into the socket.
      if (this->socket->writeDatagram(
	    reinterpret_cast<const char*>(packet.contents),
	    packet.size,
	    locus.host.location,
	    locus.port) != static_cast<qint64>(packet.size))
	escape(this->socket->errorString().toStdString().c_str());

      leave();
    }

    ///
    /// this method reads datagram(s) from the socket.
    ///
    /// note that since UDP datagrams are constrained by a maxium size,
    /// there is no need to buffer the fetched datagram hoping for the next
    /// one to complete it.
    ///
    /// therefore, whenever a datagram is fetched, the method tries to makes
    /// sense out of it. if unable, the fetched datagrams are simply dropped.
    ///
    /// indeed there is no way to extract a following datagram if
    /// the first one is invalid since datagrams are variable in size.
    ///
    /// the method returns a raw with the read data.
    ///
    Status		Slot::Read(Locus&			locus,
				   Raw&				raw)
    {
      Natural32		size;

      enter();

      // retrieve the size of the pending packet.
      size = this->socket->pendingDatagramSize();

      // check if there is data to be read.
      if (size == 0)
	leave();

      // set the locus as being an IP locus.
      if (locus.host.Create(Host::TypeIP) == StatusError)
	escape("unable to create an IP locus");

      // prepare the raw
      if (raw.Prepare(size) == StatusError)
	escape("unable to prepare the raw");

      // read the datagram from the socket.
      if (this->socket->readDatagram(
	    reinterpret_cast<char*>(raw.contents),
	    size,
	    &locus.host.location,
	    &locus.port) != size)
	escape(this->socket->errorString().toStdString().c_str());

      // set the raw's size.
      raw.size = size;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the slot's state.
    ///
    Status		Slot::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Slot]" << std::endl;

      // dump the socket.
      if (Socket::Dump(margin + 2) == StatusError)
	escape("unable to dump the socket");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback fetches parcels and dispatches them.
    ///
    Status		Slot::Dispatch()
    {
      Locus		locus;
      Natural32		offset;
      Raw		raw;

      enter();

      // read from the socket.
      if (this->Read(locus, raw) == StatusError)
	escape("unable to read from the socket");

      // initialize the offset.
      offset = 0;

      while (true)
	{
	  Packet	packet;
	  Region	frame;
	  Parcel*	parcel;

	  enterx(instance(parcel));

	  // create the frame based on the previously extracted raw.
	  if (frame.Wrap(raw.contents + offset,
			 raw.size - offset) == StatusError)
	    escape("unable to wrap a frame in the raw");

	  // prepare the packet based on the frame.
	  if (packet.Wrap(frame) == StatusError)
	    escape("unable to prepare the packet");

	  // allocate the parcel.
	  parcel = new Parcel;

	  // extract the header.
	  if (parcel->header->Extract(packet) == StatusError)
	    escape("unable to extract the header");

	  // if there is not enough data in the raw to complete the parcel...
	  if ((packet.size - packet.offset) < parcel->header->size)
	    {
	      // since the parcel will not be built, delete the instance.
	      delete parcel;

	      // waive tracking.
	      waive(parcel);

	      // exit the loop since there is not enough data anyway.
	      break;
	    }

	  // extract the data.
	  if (packet.Extract(*parcel->data) == StatusError)
	    escape("unable to extract the data");

	  // move to the next frame by setting the offset at the end of
	  // the extracted frame.
	  offset = offset + packet.offset;

	  // create the session.
	  if (parcel->session->Create(this,
				      locus,
				      parcel->header->event) == StatusError)
	    escape("unable to create the session");

	  // trigger the network shipment mechanism.
	  if (Socket::Ship(parcel) == StatusError)
	    log("an error occured while shipping the parcel");

	  // stop tracking the parcel.
	  waive(parcel);

	  // release the resources.
	  release();
	}

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered when data is ready on the socket.
    ///
    void		Slot::_ready()
    {
      Closure< Status,
	       Parameters<>
	       >	closure(Callback<>::Infer(&Signal<
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
    /// note here that the type QLocalSocket::LocalSocketError cannot be
    /// written completely ::QLocalSocket::LocalSocketError because the
    /// QT parser is incapable of recognising the type.
    ///
    void		Slot::_error(const QAbstractSocket::SocketError)
    {
      String		cause(this->socket->errorString().toStdString());
      Closure< Status,
	       Parameters<
		 const String&
		 >
	       >	closure(Callback<>::Infer(&Signal<
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
