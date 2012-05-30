//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun feb  7 01:32:45 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/tcp/Client.hh>

namespace elle
{
  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the client.
    ///
    Status              Client::Setup(const String&             line)
    {
      // create the locus.
      if (this->locus.Create(line) == Status::Error)
        escape("unable to create the locus");

      return Status::Ok;
    }

    ///
    /// this method is the thread entry locus.
    ///
    Status              Client::Run()
    {
      this->locus.Dump();

      // register the message.
      if (Network::Register(
            Procedure<TagChallenge>(
              Callback<>::Infer(&Client::Challenge, this))) == Status::Error)
        escape("unable to register the challenge message");

      // create the socket.
      if (this->socket.Create() == Status::Error)
        escape("unable to create the socket");

      // connect the socket.
      if (this->socket.Connect(this->locus) == Status::Error)
        escape("unable to connect to the bridge");

      return Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles messages.
    ///
      Status            Client::Challenge(const String&         text)
    {
      String            response("RESPONSE");

      // simply display the text.
      std::cout << "[Challenge] " << text << std::endl;

      // reply.
      if (this->socket.Reply(
            Inputs<TagResponse>(response)) == Status::Error)
        escape("unable to return the response");

      elle::concurrency::Program::Exit();

      return Status::Ok;
    }

  }
}
