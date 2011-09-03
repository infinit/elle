//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/door/Client.cc
//
// created       julien quintard   [sun feb  7 01:32:45 2010]
// updated       julien quintard   [fri sep  2 20:40:51 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/door/Client.hh>

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
    Status		Client::Setup(const String&		line)
    {
      enter();

      // set the line.
      this->line = line;

      leave();
    }

    ///
    /// this method is the thread entry point.
    ///
    Status		Client::Run()
    {
      enter();

      std::cout << "[line] " << this->line << std::endl;

      // register the message.
      if (Network::Register(
	    Procedure<TagChallenge>(
	      Callback<>::Infer(&Client::Challenge, this))) == StatusError)
	escape("unable to register the challenge message");

      // create the door.
      if (this->door.Create() == StatusError)
	escape("unable to create the slot");

      // connect the door.
      if (this->door.Connect(this->line) == StatusError)
	escape("unable to connect to the line");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles messages.
    ///
    Status		Client::Challenge(const String&		text)
    {
      String		response("RESPONSE");

      enter();

      // simply display the text.
      std::cout << "[Challenge] " << text << std::endl;

      // reply.
      if (this->door.Reply(
	    Inputs<TagResponse>(response)) == StatusError)
	escape("unable to return the response");

      leave();
    }

  }
}
