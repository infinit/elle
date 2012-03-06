//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu feb  4 15:20:31 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/LocalServer.hh>

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
    LocalServer::Container              LocalServer::Porters;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    LocalServerPorter::LocalServerPorter(const
                                           Callback<
                                             Status,
                                             Parameters<LocalSocket*>
                                             >&                 callback):
      server(NULL),
      callback(callback)
    {
    }

    ///
    /// the destructor.
    ///
    LocalServerPorter::~LocalServerPorter()
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
    Status              LocalServerPorter::Create(const String& name)
    {
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

      return StatusOk;
    }

    //
    // server
    //

    ///
    /// this method initializes the server.
    ///
    Status              LocalServer::Initialize()
    {
      // nothing to do.

      return StatusOk;
    }

    ///
    /// this method cleans the server.
    ///
    Status              LocalServer::Clean()
    {
      LocalServer::Scoutor      scoutor;

      // go through the porters.
      for (scoutor = LocalServer::Porters.begin();
           scoutor != LocalServer::Porters.end();
           scoutor++)
        {
          LocalServerPorter*    porter = scoutor->second;

          // delete the porter.
          delete porter;
        }

      return StatusOk;
    }

    ///
    /// this method starts a server and waits for new connection. for
    /// every new connection, the Accept signal is generated which, in turn,
    /// creates a new socket.
    ///
    /// note that callbacks are used because only a specific handler must
    /// be called. by relying on QT signals/slots (though it is not possible
    /// since the LocalServer class is static), all the slots registered on the
    /// signal would be triggered which is not want we want.
    ///
    Status              LocalServer::Listen(const String&       name,
                                            const
                                              Callback<
                                                Status,
                                                Parameters<LocalSocket*>
                                                >&              callback)
    {
      typedef std::unique_ptr<LocalServerPorter> ServerPtr;

      // check if this name is not already listened on.
      if (LocalServer::Locate(name) == StatusTrue)
        escape("this name seems to have already been registered");

      // allocate a new porter.
      auto porter = ServerPtr(new LocalServerPorter(callback));

      // create the porter.
      if (porter->Create(name) == StatusError)
        escape("unable to create the porter");

      // insert the porter in the container.
      auto result = LocalServer::Porters.insert(
        std::pair<const String, LocalServerPorter*>(name, porter.get()));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the porter in the container");

      // stop tracking porter.
      porter.release();

      return StatusOk;
    }

    ///
    /// this method blocks the given name by deleting the associated
    /// porter.
    ///
    Status              LocalServer::Block(const String&        name)
    {
      LocalServer::Iterator     iterator;
      LocalServerPorter*        porter;

      // locate the porter.
      if (LocalServer::Locate(name, &iterator) == StatusFalse)
        escape("unable to locate the given porter");

      // retrieve the porter.
      porter = iterator->second;

      // delete the porter.
      delete porter;

      // remove the entry from the container.
      LocalServer::Porters.erase(iterator);

      return StatusOk;
    }

    ///
    /// this method returns the porter associated with the given name.
    ///
    Status              LocalServer::Retrieve(const String&     name,
                                              LocalServerPorter*& porter)
    {
      LocalServer::Iterator     iterator;

      // locate the porter.
      if (LocalServer::Locate(name, &iterator) == StatusFalse)
        escape("unable to locate the given porter");

      // retrieve the porter.
      porter = iterator->second;

      return StatusOk;
    }

    ///
    /// this method tries to locate the porter associated with the given
    /// name and returns true if found.
    ///
    Status              LocalServer::Locate(const String&       name,
                                            Iterator*           iterator)
    {
      LocalServer::Iterator     i;

      // try to locate the porter.
      if ((i = LocalServer::Porters.find(name)) != LocalServer::Porters.end())
        {
          if (iterator != NULL)
            *iterator = i;

          return StatusTrue;
        }

      return StatusFalse;
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
    Status              LocalServerPorter::Dump(const Natural32 margin) const
    {
      String            alignment(margin, ' ');

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

      return StatusOk;
    }

    //
    // server
    //

    ///
    /// this method dumps the table of porters.
    ///
    Status              LocalServer::Show(const Natural32       margin)
    {
      String            alignment(margin, ' ');
      LocalServer::Scoutor      scoutor;

      std::cout << alignment << "[LocalServer]" << std::endl;

      // dump the porters table.
      for (scoutor = LocalServer::Porters.begin();
           scoutor != LocalServer::Porters.end();
           scoutor++)
        {
          LocalServerPorter*    porter = scoutor->second;

          // dump the porter.
          if (porter->Dump(margin + 2) == StatusError)
            escape("unable to dump the porter");
        }

      return StatusOk;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever a new conncetion is made.
    ///
    Status              LocalServerPorter::Accept()
    {
      ::QLocalSocket*   connection;

      // retrieve the connection from the server.
      if ((connection = this->server->nextPendingConnection()) == NULL)
        escape(this->server->errorString().toStdString().c_str());

      // allocate a new socket to this server.
      auto socket = std::unique_ptr<LocalSocket>(new LocalSocket);

      // create a socket with the specific connection.
      if (socket->Create(connection) == StatusError)
        escape("unable to create the socket");

      // call the callback.
      if (this->callback.Call(socket.get()) == StatusError)
        escape("an error occured in the callback");

      // stop tracking socket as it has been handed to the callback.
      socket.release();

      return StatusOk;
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered whenever a connection is being made on the
    /// porter's locus.
    ///
    void                LocalServerPorter::_accept()
    {
      Closure<
        Status,
        Parameters<>
        >               closure(Callback<>::Infer(
                                  &LocalServerPorter::Accept, this));

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
        yield(_(), "unable to spawn a fiber");
    }

 }
}
