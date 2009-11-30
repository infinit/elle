//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Socket.cc
//
// created       julien quintard   [fri nov 27 13:56:40 2009]
// updated       julien quintard   [mon nov 30 13:17:29 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Socket.hh>

namespace elle
{
  using namespace core;

  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor
    ///
    Socket::Socket():
      socket(NULL),
      port(0)
    {
    }

    ///
    /// destructor
    ///
    Socket::~Socket()
    {
      // release the socket.
      if (this->socket != NULL)
	delete this->socket;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a socket.
    ///
    Status		Socket::Create(const Natural16		port)
    {
      // allocate a new UDP socket.
      this->socket = new QUdpSocket(this);
      this->port = port;

      // bind the socket to the port.
      if (this->socket->bind(this->port) == false)
	escape("unable to bind the socket to the given port");

      // connect the signals.
      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Process())) == false)
	escape("unable to connect the 'ready read' signal");

      leave();
    }

    ///
    /// this method creates a socket on the default port.
    ///
    Status		Socket::Create()
    {
      // allocate a new UDP socket.
      this->socket = new QUdpSocket(this);

      // bind the socket.
      if (this->socket->bind() == false)
	escape("unable to bind the socket");

      // retrieve the port.
      this->port = this->socket->localPort();

      // connect the signals.
      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Process())) == false)
	escape("unable to connect the 'ready read' signal");

      leave();
    }

    ///
    /// this method sends a datagram to the specified address and port.
    ///
    Status		Socket::Send(const Location&		location,
				     const Packet&		packet)
    {
      Archive		archive;

      // create the archive that will receive both the tag and data.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the tag.
      if (archive.Serialize(packet.tag) == StatusError)
	escape("unable to serialize the tag");

      // serialize the data.
      if (packet.Serialize(archive) == StatusError)
	escape("unable to serialize the packet");

      // push the datagram into the socket.
      if (this->socket->writeDatagram((char*)archive.contents,
				      archive.size,
				      location.address.address,
				      location.port) != archive.size)
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
    /// triggers the associated callback.
    ///
    void		Socket::Process()
    {
      Location		location;
      Archive		archive;
      Socket::Scoutor	scoutor;
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
	  if (location.address.Create(Address::TypeIP) == StatusError)
	    alert("unable to create an IP address");

	  // read the datagram from the socket.
	  if (this->socket->readDatagram((char*)region.contents,
					 size,
					 &location.address.address,
					 &location.port) != size)
	    alert("unable to read the datagram");

	  // set the region's size.
	  region.size = size;

	  // detach the region.
	  if (region.Detach() == StatusError)
	    alert("unable to detach the region");

	  // create a working archive. if it fails, just skip this datagram.
	  if (archive.Prepare(region) == StatusError)
	    alert("unable to prepare the archive");

	  // extract the tag.
	  if (archive.Extract(tag) == StatusError)
	    alert("unable to extract the archive");
	}

      // retrieve the associated callback.
      scoutor = this->callbacks.find(tag);

      // check if this tag has been registered.
      if (scoutor == this->callbacks.end())
	alert("unable to locate the callback associated to the packet's tag");

      // trigger the callback.
      if (scoutor->second->Trigger(location, archive) == StatusError)
	alert("unable to trigger the callback");
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the socket state.
    ///
    Status		Socket::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Socket]" << std::endl;

      std::cout << alignment << shift << "[Address] "
		<< this->socket->localAddress().toString().toStdString()
		<< std::endl;

      std::cout << alignment << shift << "[Port] "
		<< std::dec << this->socket->localPort() << std::endl;

      leave();
    }

  }
}
