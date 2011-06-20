//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Bridge.cc
//
// created       julien quintard   [wed may 25 15:55:16 2011]
// updated       julien quintard   [sun jun 19 17:56:46 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Bridge.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

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
    BridgePorter::BridgePorter(const
			         Callback<
				   Parameters<Gate*> >&		callback):
      server(NULL),
      callback(callback)
    {
    }

    ///
    /// the destructor.
    ///
    BridgePorter::~BridgePorter()
    {
      // if there is a server, release it.
      if (this->server != NULL)
	this->server->deleteLater();
    }

//
// ---------- methods ---------------------------------------------------------
//

    //
    // porter
    //

    ///
    /// this method starts listening on the given address.
    ///
    Status		BridgePorter::Listen(const Address&	address)
    {
      enter();

      // allocate a new server.
      this->server = new ::QTcpServer;

      // start listening.
      if (this->server->listen(address.host.location, address.port) == false)
	escape(this->server->errorString().toStdString().c_str());

      // connect the signals.
      if (this->connect(this->server, SIGNAL(newConnection()),
			this, SLOT(_accept())) == false)
	escape("unable to connect the signal");

      leave();
    }

    //
    // bridge
    //

    ///
    /// this method initializes the bridge.
    ///
    Status		Bridge::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the bridge.
    ///
    Status		Bridge::Clean()
    {
      Bridge::Scoutor	scoutor;

      enter();

      // go through the porters.
      for (scoutor = Bridge::Porters.begin();
	   scoutor != Bridge::Porters.end();
	   scoutor++)
	{
	  BridgePorter*	porter = *scoutor;

	  // delete the porter.
	  delete porter;
	}

      leave();
    }

    ///
    /// this method starts a server and waits for new connection. for
    /// every new connection, the Accept signal is generated which, in turn,
    /// creates a new gate.
    ///
    /// note that callbacks are used because only a specific handler must
    /// be called. by relying on QT signals/slots (though it is not possible
    /// since the Bridge class is static), all the slots registered on the
    /// signal would be triggered which is not want we want.
    ///
    Status		Bridge::Listen(const Address&		address,
				       const
				         Callback<
					   Parameters<Gate*> >&	callback)
    {
      BridgePorter*	porter;

      enter(instance(porter));

      // allocate a new porter.
      porter = new BridgePorter(callback);

      // start listening.
      if (porter->Listen(address) == StatusError)
	escape("unable to listen on the bridge");

      // add the porter to the container.
      Bridge::Porters.push_back(porter);

      // stop tracking porter.
      waive(porter);

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
    Status		BridgePorter::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Porter]" << std::endl;

      // dump the server address.
      std::cout << alignment << Dumpable::Shift << "[Server] "
		<< std::hex << this->server << std::endl;

      // XXX

      // dump the callback.
      if (this->callback.Dump(margin + 2) == StatusError)
	escape("unable to dump the callback");

      leave();
    }

    //
    // bridge
    //

    ///
    /// this method dumps the table of porters.
    ///
    Status		Bridge::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      Bridge::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Bridge]" << std::endl;

      // dump the porters table.
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
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever a new conncetion is made.
    ///
    Status		BridgePorter::Accept()
    {
      ::QTcpSocket*	socket;
      Gate*		gate;

      enter(instance(gate));

      // retrieve the socket from the server.
      if ((socket = this->server->nextPendingConnection()) == NULL)
	escape(this->server->errorString().toStdString().c_str());

      // allocate a new gate to this bridge.
      /// XXX \todo we should be able to specify the mode somewhere.
      gate = new Gate(Socket::ModeAsynchronous);

      // create a gate with the specific socket.
      if (gate->Create(socket) == StatusError)
	escape("unable to create the gate");

      // trigger the callback.
      if (this->callback.Trigger(gate) == StatusError)
	escape("an error occured in the callback");

      // stop tracking gate as it has been handed to the callback.
      waive(gate);

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    void		BridgePorter::_accept()
    {
      Callback< Parameters<> >	callback(&BridgePorter::Accept, this);
      Closure< Parameters<> >	closure(callback);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert("unable to spawn a fiber");

      release();
    }

 }
}
