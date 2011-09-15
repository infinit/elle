//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed may 25 15:55:16 2011]
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
				   Status,
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
      // stop listening.
      this->server->close();

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
    /// this method creates and starts the porter.
    ///
    Status		BridgePorter::Create(const Locus&	locus)
    {
      enter();

      // set the locus.
      this->locus = locus;

      // allocate a new server.
      this->server = new ::QTcpServer;

      // start listening.
      if (this->server->listen(this->locus.host.location,
			       this->locus.port) == false)
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
	  BridgePorter*	porter = scoutor->second;

	  // delete the porter.
	  delete porter;
	}

      // clear the container.
      Bridge::Porters.clear();

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
    Status		Bridge::Listen(const Locus&		locus,
				       const
				         Callback<
					   Status,
					   Parameters<Gate*> >&	callback)
    {
      std::pair<Bridge::Iterator, Boolean>	result;
      BridgePorter*				porter;

      enter(instance(porter));

      // check if this locus is not already listened on.
      if (Bridge::Locate(locus) == StatusTrue)
	escape("this locus seems to have already been registered");

      // allocate a new porter.
      porter = new BridgePorter(callback);

      // create the porter.
      if (porter->Create(locus) == StatusError)
	escape("unable to create the porter");

      // insert the porter in the container.
      result = Bridge::Porters.insert(
	         std::pair<const Locus, BridgePorter*>(locus, porter));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the porter in the container");

      // stop tracking porter.
      waive(porter);

      leave();
    }

    ///
    /// this method blocks the given locus by deleting the associated
    /// porter.
    ///
    Status		Bridge::Block(const Locus&		locus)
    {
      Bridge::Iterator	iterator;
      BridgePorter*	porter;

      enter();

      // locate the porter.
      if (Bridge::Locate(locus, &iterator) == StatusFalse)
	escape("unable to locate the given porter");

      // retrieve the porter.
      porter = iterator->second;
      
      // delete the porter.
      delete porter;

      // remove the entry from the container.
      Bridge::Porters.erase(iterator);

      leave();
    }

    ///
    /// this method returns the porter associated with the given locus.
    ///
    Status		Bridge::Retrieve(const Locus&		locus,
					 BridgePorter*&		porter)
    {
      Bridge::Iterator	iterator;

      enter();

      // locate the porter.
      if (Bridge::Locate(locus, &iterator) == StatusFalse)
	escape("unable to locate the given porter");

      // retrieve the porter.
      porter = iterator->second;

      leave();
    }

    ///
    /// this method tries to locate the porter associated with the given
    /// locus and returns true if so.
    ///
    Status		Bridge::Locate(const Locus&		locus,
				       Iterator*		iterator)
    {
      Bridge::Iterator	i;

      enter();

      // try to locate the porter.
      if ((i = Bridge::Porters.find(locus)) != Bridge::Porters.end())
	{
	  if (iterator != NULL)
	    *iterator = i;

	  true();
	}

      false();
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

      // dump the locus.
      if (this->locus.Dump(margin + 2) == StatusError)
	escape("unable to dump the locus");

      // dump the server locus.
      std::cout << alignment << Dumpable::Shift << "[Server] "
		<< std::hex << this->server << std::endl;

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
	  BridgePorter*	porter = scoutor->second;

	  // dump the porter.
	  if (porter->Dump(margin + 2) == StatusError)
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
      gate = new Gate;

      // create a gate with the specific socket.
      if (gate->Create(socket) == StatusError)
	escape("unable to create the gate");

      // call the callback.
      if (this->callback.Call(gate) == StatusError)
	escape("an error occured in the callback");

      // stop tracking gate as it has been handed to the callback.
      waive(gate);

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered whenever a connection is being made on the
    /// porter's locus.
    ///
    void		BridgePorter::_accept()
    {
      Closure< Status,
	       Parameters<> >	closure(Callback<>::Infer(
					  &BridgePorter::Accept, this));

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	yield(_(), "unable to spawn a fiber");

      release();
    }

 }
}
