//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Slot.cc
//
// created       julien quintard   [wed feb  3 21:52:30 2010]
// updated       julien quintard   [thu jul 14 14:15:51 2011]
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

      socket(NULL),
      port(0)
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
    /// this method creates a slot with a specific port.
    ///
    Status		Slot::Create(const Port			port)
    {
      enter();

      // allocate a new UDP socket.
      this->socket = new ::QUdpSocket;

      // set the port.
      this->port = port;

      // bind the socket to the port.
      if (this->socket->bind(this->port) == false)
	escape(this->socket->errorString().toStdString().c_str());

      // connect the signals.
      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Fetch())) == false)
	escape("unable to connect the signal");

      leave();
    }

    ///
    /// this method writes a packet on the socket so that it gets sent
    /// to the given address.
    ///
    Status		Slot::Write(const Address&		address,
				    const Packet&		packet)
    {
      enter();

      // push the datagram into the socket.
      if (this->socket->writeDatagram((char*)packet.contents,
				      packet.size,
				      address.host.location,
				      address.port) != (::qint64)packet.size)
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
    Status		Slot::Read(Address&			address,
				   Raw&				raw)
    {
      Natural32		size;

      enter();

      // retrieve the size of the pending packet.
      size = this->socket->pendingDatagramSize();

      // check if there is data to be read.
      if (size == 0)
	leave();

      // set the address as being an IP address.
      if (address.host.Create(Host::TypeIP) == StatusError)
	escape("unable to create an IP address");

      // prepare the raw
      if (raw.Prepare(size) == StatusError)
	escape("unable to prepare the raw");

      // read the datagram from the socket.
      if (this->socket->readDatagram((char*)raw.contents,
				     size,
				     &address.host.location,
				     &address.port) != size)
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

      // XXX hasPendingDatagrams, BindMode, SocketState, Valid,
      // XXX TransportProtocol, ...

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever the socket changes state or
    /// if an error occurs.
    ///
    Status		Slot::Error(const String&		text)
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
    Status		Slot::Dispatch()
    {
      Address		address;
      Natural32		offset;
      Raw		raw;

      enter();

      // read from the socket.
      if (this->Read(address, raw) == StatusError)
	escape("unable to read from the socket");

      // initialize the offset.
      offset = 0;

      while (true)
	{
	  Region	frame;
	  Packet	packet;
	  Parcel*	parcel;

	  enter(instance(parcel));

	  // create the frame based on the previously extracted raw.
	  if (frame.Wrap(raw.contents + offset,
			 raw.size - offset) == StatusError)
	    escape("unable to wrap a frame in the raw");

	  // prepare the packet based on the frame.
	  if (packet.Prepare(frame) == StatusError)
	    escape("unable to prepare the packet");

	  // detach the frame from the packet so that the region is
	  // not released once the packet is destroyed. indeed, since the frame
	  // is a chunk, it must not be freed.
	  if (packet.Detach() == StatusError)
	    escape("unable to detach the data from the packet");

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
				      address,
				      parcel->header->event) == StatusError)
	    escape("unable to create the session");

	  // dispatch this parcel to the network manager.
	  //
	  // note that a this point, the network is longer responsible
	  // for the parcel and its memory.
	  if (Network::Dispatch(parcel) == StatusError)
	    {
	      Report*		report;
	      Session*		session;

	      // retrieve the report.
	      if (Report::Instance(report) == StatusFalse)
		escape("unable to retrieve the report");

	      // retrieve the session.
	      if (Session::Instance(session) == StatusError)
		escape("unable to retrieve the session");

	      // since an error occured, transmit it to the sender
	      if (this->Send(session->address,
			     Inputs<TagError>(*report)) == StatusError)
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

	  // release the resources.
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
    void		Slot::_error(const QAbstractSocket::SocketError)
    {
      String		text(this->socket->errorString().toStdString());
      Callback< Parameters<const String> >	callback(&Slot::Error, this);
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
    void		Slot::_ready()
    {
      Callback< Parameters<> >	callback(&Slot::Dispatch, this);
      Closure< Parameters<> >	closure(callback);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert(_(), "unable to spawn a fiber");

      release();
    }

  }
}
