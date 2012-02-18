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
      if (this->locus.Create(line) == StatusError)
        escape("unable to create the locus");

      return elle::StatusOk;
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
              Callback<>::Infer(&Client::Challenge, this))) == StatusError)
        escape("unable to register the challenge message");

      // create the socket.
      if (this->socket.Create() == StatusError)
        escape("unable to create the socket");

      // connect the socket.
      if (this->socket.Connect(this->locus) == StatusError)
        escape("unable to connect to the bridge");

      return elle::StatusOk;
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
            Inputs<TagResponse>(response)) == StatusError)
        escape("unable to return the response");

      return elle::StatusOk;
    }

  }
}
