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
// updated       julien quintard   [sun feb 21 21:00:31 2010]
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

      return (this->Create(socket));
    }

    ///
    /// this method creates a door based on the given socket.
    ///
    Status		Door::Create(::QLocalSocket*		socket)
    {
      // set the socket.
      this->socket = socket;

      // connect the signals.
      if (this->connect(this->socket, SIGNAL(disconnected()),
			this, SIGNAL(Disconnected())) == false)
	escape("unable to connect the signal");

      if (this->connect(this->socket, SIGNAL(readyRead()),
			this, SLOT(Read())) == false)
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
      // disconnect the socket from the server.
      this->socket->disconnectFromServer();

      leave();
    }

    ///
    /// XXX
    ///
    /* XXX
    Status		Door::Send(const Message&		message)
    {
      Archive		archive;

      // create the archive that will receive both the tag and data.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the message.
      if (archive.Serialize(message) == StatusError)
	escape("unable to serialize the message");

      // push the packet to the socket.
      if (this->socket->write((const char*)archive.contents,
			      archive.size) == StatusError)
	escape("unable to write the packet");

      // flush to start sending data immediately.
      this->socket->flush();

      leave();
    }
    */

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
      // basically just report the error without doing anything else.
      alert(this->socket->errorString().toStdString().c_str());
    }

    ///
    /// this slot is triggered whenever data is available on the socket.
    ///
    void		Door::Read()
    {
      Environment	environment(*this);
      Archive		archive;
      Region		region;
      Natural64		size;

      // retrieve the size of the data available.
      size = this->socket->bytesAvailable();

      // prepare the region.
      if (region.Prepare(size) == StatusError)
	alert("unable to prepare the region");

      // read the packet from the socket.
      if (this->socket->read((char*)region.contents, size) != size)
	alert(this->socket->errorString().toStdString().c_str());

      // set the region's size.
      region.size = size;

      // detach the region.
      if (region.Detach() == StatusError)
	alert("unable to detach the region");

      // create a working archive. if it fails, just skip this datagram.
      if (archive.Prepare(region) == StatusError)
	alert("unable to prepare the archive");

      // dispatch the event.
      if (Network::Dispatch(environment, archive) == StatusError)
	alert("unable to dispatch the event");
    }

  }
}
