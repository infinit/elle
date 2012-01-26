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

#include <elle/network/TCPServer.hh>

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
    TCPServer::Container                TCPServer::Porters;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    TCPServerPorter::TCPServerPorter(const
                                       Callback<
                                         Status,
                                         Parameters<TCPSocket*>
                                         >&                     callback):
      server(NULL),
      callback(callback)
    {
    }

    ///
    /// the destructor.
    ///
    TCPServerPorter::~TCPServerPorter()
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
    Status              TCPServerPorter::Create(const Locus&    locus)
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
    // server
    //

    ///
    /// this method initializes the server.
    ///
    Status              TCPServer::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the server.
    ///
    Status              TCPServer::Clean()
    {
      TCPServer::Scoutor        scoutor;

      enter();

      // go through the porters.
      for (scoutor = TCPServer::Porters.begin();
           scoutor != TCPServer::Porters.end();
           scoutor++)
        {
          TCPServerPorter*      porter = scoutor->second;

          // delete the porter.
          delete porter;
        }

      // clear the container.
      TCPServer::Porters.clear();

      leave();
    }

    ///
    /// this method starts a server and waits for new connection. for
    /// every new connection, the Accept signal is generated which, in turn,
    /// creates a new TCP socket.
    ///
    /// note that callbacks are used because only a specific handler must
    /// be called. by relying on QT signals/slots (though it is not possible
    /// since the TCPServer class is static), all the slots registered on the
    /// signal would be triggered which is not want we want.
    ///
    Status              TCPServer::Listen(const Locus&          locus,
                                          const
                                            Callback<
                                              Status,
                                              Parameters<TCPSocket*>
                                              >&                callback)
    {
      std::pair<TCPServer::Iterator, Boolean>   result;
      TCPServerPorter*                          porter;

      enterx(instance(porter));

      // check if this locus is not already listened on.
      if (TCPServer::Locate(locus) == StatusTrue)
        escape("this locus seems to have already been registered");

      // allocate a new porter.
      porter = new TCPServerPorter(callback);

      // create the porter.
      if (porter->Create(locus) == StatusError)
        escape("unable to create the porter");

      // insert the porter in the container.
      result = TCPServer::Porters.insert(
                 std::pair<const Locus, TCPServerPorter*>(locus, porter));

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
    Status              TCPServer::Block(const Locus&           locus)
    {
      TCPServer::Iterator       iterator;
      TCPServerPorter*          porter;

      enter();

      // locate the porter.
      if (TCPServer::Locate(locus, &iterator) == StatusFalse)
        escape("unable to locate the given porter");

      // retrieve the porter.
      porter = iterator->second;
      
      // delete the porter.
      delete porter;

      // remove the entry from the container.
      TCPServer::Porters.erase(iterator);

      leave();
    }

    ///
    /// this method returns the porter associated with the given locus.
    ///
    Status              TCPServer::Retrieve(const Locus&        locus,
                                            TCPServerPorter*&   porter)
    {
      TCPServer::Iterator       iterator;

      enter();

      // locate the porter.
      if (TCPServer::Locate(locus, &iterator) == StatusFalse)
        escape("unable to locate the given porter");

      // retrieve the porter.
      porter = iterator->second;

      leave();
    }

    ///
    /// this method tries to locate the porter associated with the given
    /// locus and returns true if so.
    ///
    Status              TCPServer::Locate(const Locus&          locus,
                                          Iterator*             iterator)
    {
      TCPServer::Iterator       i;

      enter();

      // try to locate the porter.
      if ((i = TCPServer::Porters.find(locus)) != TCPServer::Porters.end())
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
    Status              TCPServerPorter::Dump(const Natural32   margin) const
    {
      String            alignment(margin, ' ');

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
    // server
    //

    ///
    /// this method dumps the table of porters.
    ///
    Status              TCPServer::Show(const Natural32         margin)
    {
      String                    alignment(margin, ' ');
      TCPServer::Scoutor        scoutor;

      enter();

      std::cout << alignment << "[TCPServer]" << std::endl;

      // dump the porters table.
      for (scoutor = TCPServer::Porters.begin();
           scoutor != TCPServer::Porters.end();
           scoutor++)
        {
          TCPServerPorter*      porter = scoutor->second;

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
    Status              TCPServerPorter::Accept()
    {
      ::QTcpSocket*     connection;
      TCPSocket*        socket;

      enterx(instance(socket));

      // retrieve the connection from the server.
      if ((connection = this->server->nextPendingConnection()) == NULL)
        escape(this->server->errorString().toStdString().c_str());

      // allocate a new socket to this server.
      socket = new TCPSocket;

      // create a socket with the specific connection.
      if (socket->Create(connection) == StatusError)
        escape("unable to create the socket");

      // call the callback.
      if (this->callback.Call(socket) == StatusError)
        escape("an error occured in the callback");

      // stop tracking socket as it has been handed to the callback.
      waive(socket);

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered whenever a connection is being made on the
    /// porter's locus.
    ///
    void                TCPServerPorter::_accept()
    {
      Closure<
        Status,
        Parameters<>
        >               closure(Callback<>::Infer(
                                  &TCPServerPorter::Accept, this));

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
        yield(_(), "unable to spawn a fiber");

      release();
    }

 }
}
