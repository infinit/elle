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
// updated       julien quintard   [sun feb 28 11:03:29 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Slot.hh>

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
      Socket(Socket::TypeSlot),

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
	delete this->socket;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Slot::Create()
    {
      // allocate a new UDP socket.
      this->socket = new ::QUdpSocket(this);

      // bind the socket.
      if (this->socket->bind() == false)
	escape(this->socket->errorString().toStdString().c_str());

      // retrieve the port.
      this->port = this->socket->localPort();

      // connect the signals.
      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Read())) == false)
	escape("unable to connect the signal");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Slot::Create(const Port			port)
    {
      // allocate a new UDP socket.
      this->socket = new ::QUdpSocket(this);
      this->port = port;

      // bind the socket to the port.
      if (this->socket->bind(this->port) == false)
	escape(this->socket->errorString().toStdString().c_str());

      // connect the signals.
      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Read())) == false)
	escape("unable to connect the signal");

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is called whenever a datagram is ready on the socket.
    ///
    /// this method reads the datagram, reconstitute the archive and
    /// dispatch the event.
    ///
    void		Slot::Read()
    {
      Address		address;
      Packet		packet;
      Tag		tag;

      // read the pending datagrams.
      while (this->socket->hasPendingDatagrams() == true)
	{
	  Region		region;
	  Natural32		size;

	  // retrieve the size of the pending packet.
	  size = this->socket->pendingDatagramSize();

	  // prepare the region.
	  if (region.Prepare(size) == StatusError)
	    alert("unable to prepare the region");

	  // set the address as being an IP address.
	  if (address.host.Create(Host::TypeIP) == StatusError)
	    alert("unable to create an IP address");

	  // read the datagram from the socket.
	  if (this->socket->readDatagram((char*)region.contents,
					 size,
					 &address.host.location,
					 &address.port) != size)
	    alert(this->socket->errorString().toStdString().c_str());

	  // set the region's size.
	  region.size = size;

	  // detach the region.
	  if (region.Detach() == StatusError)
	    alert("unable to detach the region");

	  // create a working packet. if it fails, just skip this datagram.
	  if (packet.Prepare(region) == StatusError)
	    alert("unable to prepare the packet");
	}

      // initialize the context.
      if (Context::Initialize(this, address) == StatusError)
	alert("unable to initialize the context");

      // dispatch the event.
      if (Network::Dispatch(packet) == StatusError)
	alert("unable to dispatch the event");
    }

//
// ---------- dumpable --------------------------------------------------------
//

    // XXX

  }
}
