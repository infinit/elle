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
// updated       julien quintard   [sat feb  6 05:11:32 2010]
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
	escape("unable to bind the socket");

      // retrieve the port.
      this->port = this->socket->localPort();

      // connect the signals.
      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Read())) == false)
	escape("unable to connect the 'ready read' signal");

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
	escape("unable to bind the socket to the given port");

      // connect the signals.
      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Read())) == false)
	escape("unable to connect the 'ready read' signal");

      leave();
    }

    ///
    /// this message converts a message into a UDP packet and sends it
    /// to the given address.
    ///
    Status		Slot::Send(const Message&		message,
				   const Address&		address)
    {
      Archive		archive;

      // create the archive that will receive both the tag and data.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the message.
      if (archive.Serialize(message) == StatusError)
	escape("unable to serialize the message");

      // push the datagram into the socket.
      if (this->socket->writeDatagram((char*)archive.contents,
				      archive.size,
				      address.host.location,
				      address.port) != archive.size)
	escape("unable to send the datagram");

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
      Environment	environment(*this);
      Archive		archive;
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

	  // set the location as being an IP address.
	  if (environment.address.host.Create(Host::TypeIP) == StatusError)
	    alert("unable to create an IP address");

	  // read the datagram from the socket.
	  if (this->socket->readDatagram((char*)region.contents,
					 size,
					 &environment.address.host.location,
					 &environment.address.port) != size)
	    alert("unable to read the datagram");

	  // set the region's size.
	  region.size = size;

	  // detach the region.
	  if (region.Detach() == StatusError)
	    alert("unable to detach the region");

	  // create a working archive. if it fails, just skip this datagram.
	  if (archive.Prepare(region) == StatusError)
	    alert("unable to prepare the archive");
	}

      // dispatch the event.
      if (Network::Dispatch(environment, archive) == StatusError)
	alert("unable to dispatch the event");
    }

//
// ---------- dumpable --------------------------------------------------------
//

    // XXX

  }
}
