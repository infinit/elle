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
// updated       julien quintard   [sat feb  6 05:51:55 2010]
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
    Bridge::Container		Bridge::Listeners;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Bridge::Listener::Listener():
      callback(NULL)
    {
    }

    ///
    /// the destructor.
    ///
    Bridge::Listener::~Listener()
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
    // listener
    //

    ///
    /// this method starts listening on the given name.
    ///
    Status		Bridge::Listener::Create(const String&	name,
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
    Status		Bridge::Listen(const String&		name,
				       Callback*		callback)
    {
      Bridge::Listener*	listener;

      // allocate a new listener.
      listener = new Bridge::Listener;

      // create the listener.
      if (listener->Create(name, callback) == StatusError)
	escape("unable to create the listener");

      // add the listener to the container.
      Bridge::Listeners.push_back(listener);

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the internals of a listener.
    ///
    Status		Bridge::Listener::Dump(const Natural32	margin) const
    {
      // XXX

      leave();
    }

    ///
    /// this method dumps the table of listeners.
    ///
    Status		Bridge::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      Bridge::Scoutor	scoutor;

      std::cout << alignment << "[Bridge]" << std::endl;

      // dump the callbacks table.
      for (scoutor = Bridge::Listeners.begin();
	   scoutor != Bridge::Listeners.end();
	   scoutor++)
	{
	  // dump the listener.
	  if ((*scoutor)->Dump(margin + 2) == StatusError)
	    escape("unable to dump the listener");
	}

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered whenever a new conncetion is made.
    ///
    void		Bridge::Listener::Accept()
    {
      ::QLocalSocket*	socket;
      Door*		door;

      // retrieve the socket from the server.
      if ((socket = this->server->nextPendingConnection()) == NULL)
	alert("unable to retrive the pending connection");

      // allocate a new door to this bridge.
      door = new Door;

      // create a door with the specific socket.
      if (door->Create(socket) == StatusError)
	alert("unable to create the door");
    }

 }
}
