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
// updated       julien quintard   [sat feb  6 06:06:38 2010]
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

      // allocate a new socket.
      socket = new ::QLocalSocket(this);

      return (this->Create(this->socket));
    }

    ///
    /// XXX
    ///
    Status		Door::Create(::QLocalSocket*		socket)
    {
      // set the socket.
      this->socket = socket;

      // connect the signals.
      this->connect(this->socket, SIGNAL(disconnected()),
		    this, SIGNAL(Disconnected()));
      this->connect(this->socket, SIGNAL(readyRead()),
		    this, SLOT(Read()));
      this->connect(this->socket,
		    SIGNAL(error(QLocalSocket::LocalSocketError)),
		    this,
		    SLOT(Error(QLocalSocket::LocalSocketError)));

      leave();
    }

    ///
    /// XXX
    ///
    Status		Door::Connect(const String&		name)
    {
      // connect the socket to the server.
      this->socket->connectToServer(name.c_str());

      // wait for the socket to connect.
      if (this->socket->waitForConnected(Door::Timeout) == false)
	escape("unable to connect to the bridge");

      leave();
    }

  }
}
