//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri nov 27 22:04:36 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/tcp/Server.hh>

namespace elle
{
  namespace test
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Server::Server():
      socket(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Server::~Server()
    {
      // if present, delete the socket.
      if (this->socket != NULL)
        delete this->socket;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the server.
    ///
    Status              Server::Setup(const String&             line)
    {
      // set the locus.
      if (this->locus.Create(line) == Status::Error)
        escape("unable to create the locus");

      return Status::Ok;
    }

    ///
    /// this method is the thread entry locus.
    ///
    Status              Server::Run()
    {
      locus.Dump();

      // listen for incoming connections.
      if (TCPServer::Listen(locus,
                            Callback<>::Infer(&Server::Connection,
                                              this)) == Status::Error)
        escape("unable to listen for TCP connections");

      return Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles new connections.
    ///
    Status              Server::Connection(TCPSocket*           socket)
    {
      String            challenge("CHALLENGE");
      String            response;

      // if there is already a client, exit.
      if (this->socket != NULL)
        escape("there is already a client connected");

      // add the socket.
      this->socket = socket;

      std::cout << "[challenging...] " << std::endl;

      // call the challenge.
      if (socket->Call(Inputs<TagChallenge>(challenge),
                       Outputs<TagResponse>(response)) == Status::Error)
        escape("unable to call the challenge");

      std::cout << "[response] " << response << std::endl;

      // check the response
      if (response != elle::String("RESPONSE"))
        std::cerr << "unexpected response '" << response << "'" << std::endl;

      elle::concurrency::Program::Exit();

      return Status::Ok;
    }

  }
}
