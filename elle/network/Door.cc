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
// updated       julien quintard   [sun mar  7 16:52:11 2010]
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

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Door::Door():
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
	delete this->socket;
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
			this, SLOT(Deliver())) == false)
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
    /// this method reads the next packet from the socket.
    ///
    Status		Door::Read(Packet&			packet)
    {
      /*
      Region		region;
      Natural64		size;

      enter();

      // retrieve the size of the data available.
      size = this->socket->bytesAvailable();

      printf("Door::Read(%u available)\n", size);

      // prepare the region.
      if (region.Prepare(size) == StatusError)
	escape("unable to prepare the region");

      // read the packet from the socket.
      if (this->socket->read((char*)region.contents, size) != size)
	escape(this->socket->errorString().toStdString().c_str());

      // set the region's size.
      region.size = size;

      // detach the region.
      if (region.Detach() == StatusError)
	escape("unable to detach the region");

      // create a working packet. if it fails, just skip this datagram.
      if (packet.Prepare(region) == StatusError)
	escape("unable to prepare the packet");

      // assign the context.
      // XXX +add Identifier in context
      //if (Context::Assign(this, Address::Null) == StatusError)
      //escape("unable to assign the context");

      leave();
      */
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
    /// this slot is triggered whenever data is available on the socket.
    ///
    void		Door::Deliver()
    {
      /*
      Packet		packet;

      printf("Door::Deliver()\n");

      enter();

      // read the next packet.
      if (this->Read(packet) == StatusError)
	alert("unable to read the next packet");

      // dispatch the event.
      if (Network::Dispatch(packet) == StatusError)
	alert("unable to dispatch the event");

      release();
      */
    }

  }
}
