//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Lane.cc
//
// created       julien quintard   [thu feb  4 15:20:31 2010]
// updated       julien quintard   [mon jul 18 11:29:02 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Lane.hh>

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
    Lane::Container		Lane::Porters;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    LanePorter::LanePorter(const Callback< Status,
					   Parameters<Door*> >&	callback):
      server(NULL),
      callback(callback)
    {
    }

    ///
    /// the destructor.
    ///
    LanePorter::~LanePorter()
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
    /// this method starts listening on the given name.
    ///
    Status		LanePorter::Create(const String&		name)
    {
      enter();

      // set the name.
      this->name = name;

      // allocate a new server.
      this->server = new ::QLocalServer;

      // start listening.
      if (this->server->listen(this->name.c_str()) == false)
	escape(this->server->errorString().toStdString().c_str());

      // connect the signals.
      if (this->connect(this->server, SIGNAL(newConnection()),
			this, SLOT(_accept())) == false)
	escape("unable to connect the signal");

      leave();
    }

    //
    // lane
    //

    ///
    /// this method initializes the lane.
    ///
    Status		Lane::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the lane.
    ///
    Status		Lane::Clean()
    {
      Lane::Scoutor	scoutor;

      enter();

      // go through the porters.
      for (scoutor = Lane::Porters.begin();
	   scoutor != Lane::Porters.end();
	   scoutor++)
	{
	  LanePorter*	porter = *scoutor;

	  // delete the porter.
	  delete porter;
	}

      leave();
    }

    ///
    /// this method starts a server and waits for new connection. for
    /// every new connection, the Accept signal is generated which, in turn,
    /// creates a new door.
    ///
    /// note that callbacks are used because only a specific handler must
    /// be called. by relying on QT signals/slots (though it is not possible
    /// since the Lane class is static), all the slots registered on the
    /// signal would be triggered which is not want we want.
    ///
    Status		Lane::Listen(const String&		name,
				     const
				       Callback<
					 Status,
					 Parameters<Door*> >&	callback)
    {
      LanePorter*	porter;

      enter(instance(porter));

      // allocate a new porter.
      porter = new LanePorter(callback);

      // create the porter.
      if (porter->Create(name) == StatusError)
	escape("unable to create the porter");

      // add the porter to the container.
      Lane::Porters.push_back(porter);

      // stop tracking porter.
      waive(porter);

      leave();
    }

    ///
    /// this method blocks the given address by deleting the associated
    /// porter.
    ///
    Status		Lane::Block(const String&		name)
    {
      Lane::Iterator	iterator;

      enter();

      // go through the porters.
      for (iterator = Lane::Porters.begin();
	   iterator != Lane::Porters.end();
	   iterator++)
	{
	  LanePorter*	porter = *iterator;

	  // is this the porter we are looking for?
	  if (porter->name == name)
	    {
	      // delete the porter.
	      delete porter;

	      // remove the entry from the container.
	      Lane::Porters.erase(iterator);

	      leave();
	    }
	}

      escape("unable to locate the porter associated with this name");
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
    Status		LanePorter::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Porter]" << std::endl;

      // dump the server address.
      std::cout << alignment << Dumpable::Shift << "[Server] "
		<< std::hex << this->server << std::endl;

      // dump the platform-specific path.
      std::cout << alignment << Dumpable::Shift << "[Path] "
		<< this->server->fullServerName().toStdString() << std::endl;

      // dump the callback.
      if (this->callback.Dump(margin + 2) == StatusError)
	escape("unable to dump the callback");

      leave();
    }

    //
    // lane
    //

    ///
    /// this method dumps the table of porters.
    ///
    Status		Lane::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      Lane::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Lane]" << std::endl;

      // dump the porters table.
      for (scoutor = Lane::Porters.begin();
	   scoutor != Lane::Porters.end();
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
    Status		LanePorter::Accept()
    {
      ::QLocalSocket*	socket;
      Door*		door;

      enter(instance(door));

      // retrieve the socket from the server.
      if ((socket = this->server->nextPendingConnection()) == NULL)
	escape(this->server->errorString().toStdString().c_str());

      // allocate a new door to this lane.
      door = new Door;

      // create a door with the specific socket.
      if (door->Create(socket) == StatusError)
	escape("unable to create the door");

      // call the callback.
      if (this->callback.Call(door) == StatusError)
	escape("an error occured in the callback");

      // stop tracking door as it has been handed to the callback.
      waive(door);

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    void		LanePorter::_accept()
    {
      Callback< Status,
		Parameters<> >	callback(&LanePorter::Accept, this);
      Closure< Status,
	       Parameters<> >	closure(callback);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert(_(), "unable to spawn a fiber");

      release();
    }

 }
}
