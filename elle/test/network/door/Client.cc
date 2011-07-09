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
// updated       julien quintard   [wed jul  6 15:39:35 2011]
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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Client::Client():
      door(Socket::ModeAsynchronous)
    {
    }

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
      Callback< Parameters<const String> >	challenge(&Client::Challenge,
							  this);

      enter();

      std::cout << "[line] " << this->line << std::endl;

      // register the message.
      if (Network::Register<TagChallenge>(challenge) == StatusError)
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

      // respond.
      if (this->door.Reply(Inputs<TagResponse>(response)) == StatusError)
	escape("unable to reply to the challenge");

      leave();
    }

  }
}
