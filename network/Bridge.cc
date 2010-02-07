//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Bridge.cc
//
// created       julien quintard   [thu feb  4 15:20:31 2010]
// updated       julien quintard   [sun feb  7 02:44:23 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Bridge.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// definition of the container.
    ///
    Bridge::Container		Bridge::Porters;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Porter::Porter():
      server(NULL),
      callback(NULL)
    {
    }

    ///
    /// the destructor.
    ///
    Porter::~Porter()
    {
      // if there is a callback, release it.
      if (this->server != NULL)
	delete this->server;

      // if there is a callback, release it.
      if (this->callback != NULL)
	delete this->callback;
    }

//
// ---------- methods ---------------------------------------------------------
//

    //
    // porter
    //

    ///
    /// this method starts listening on the given name.
    ///
    Status		Porter::Create(const String&	name,
						 Callback*	callback)
    {
      // allocate a new server.
      this->server = new ::QLocalServer(this);

      // set the callback.
      this->callback = callback;

      // start listening.
      if (this->server->listen(name.c_str()) == false)
	escape(this->server->errorString().toStdString().c_str());

      // connect the signals.
      this->connect(this->server, SIGNAL(newConnection()),
		    this, SLOT(Accept()));

      leave();
    }

    //
    // bridge
    //

    ///
    /// this method starts a server and waits for new connection. for
    /// every new connection, the Accept signal is generated which, in turn,
    /// creates a new door.
    ///
    /// note that callbacks are used because only a specific handler must
    /// be called. by relying on QT signals/slots (though it is not possible
    /// since the Bridge class is static), all the slots registered on the
    /// signal would be triggered which is not want we want.
    ///
    Status		Bridge::Listen(const String&		name,
				       Callback*		callback)
    {
      Porter*	porter;

      // allocate a new porter.
      porter = new Porter;

      // create the porter.
      if (porter->Create(name, callback) == StatusError)
	escape("unable to create the porter");

      // add the porter to the container.
      Bridge::Porters.push_back(porter);

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    //
    // porter
    //

    ///
    /// this method dumps the internals of a porter.
    ///
    Status		Porter::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Porter]" << std::endl;

      // dump the server address.
      std::cout << alignment << shift << "[Server] "
		<< std::hex << this->server << std::endl;

      // dump the platform-specific path.
      std::cout << alignment << shift << "[Path] "
		<< this->server->fullServerName().toStdString() << std::endl;

      // dump the callback.
      if (this->callback->Dump(margin + 2) == StatusError)
	escape("unable to dump the callback");

      leave();
    }

    //
    // bridge
    //

    ///
    /// this method dumps the table of porters.
    ///
    Status		Bridge::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      Bridge::Scoutor	scoutor;

      std::cout << alignment << "[Bridge]" << std::endl;

      // dump the callbacks table.
      for (scoutor = Bridge::Porters.begin();
	   scoutor != Bridge::Porters.end();
	   scoutor++)
	{
	  // dump the porter.
	  if ((*scoutor)->Dump(margin + 2) == StatusError)
	    escape("unable to dump the porter");
	}

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered whenever a new conncetion is made.
    ///
    void		Porter::Accept()
    {
      ::QLocalSocket*	socket;
      Door*		door;

      // retrieve the socket from the server.
      if ((socket = this->server->nextPendingConnection()) == NULL)
	alert(this->server->errorString().toStdString().c_str());

      // allocate a new door to this bridge.
      door = new Door;

      // create a door with the specific socket.
      if (door->Create(socket) == StatusError)
	alert("unable to create the door");

      // trigger the associated callback.
      if (this->callback->Trigger(door) == StatusError)
	alert("unable to trigger the callback");
    }

 }
}
