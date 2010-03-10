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
// updated       julien quintard   [wed mar 10 19:53:11 2010]
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
    /// this method is the thread entry point.
    ///
    Status		Client::Run()
    {
      static Method<String> challenge(this, &Client::Challenge);

      enter();

      std::cout << "[bridge] " << this->name << std::endl;

      // register the message.
      if (Network::Register<TagChallenge>(challenge) == StatusError)
	escape("unable to register the challenge message");

      // create the door.
      if (this->door.Create() == StatusError)
	escape("unable to create the slot");

      // connect the door.
      if (this->door.Connect(this->name) == StatusError)
	escape("unable to connect to the bridge");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles messages.
    ///
    Status		Client::Challenge(String&			text)
    {
      enter();

      // simply display the text.
      std::cout << "[Challenge] " << text << std::endl;

      // respond.
      if (this->door.Send(Inputs<TagResponse>(text)) == StatusError)
	escape("unable to reply to the challenge");

      leave();
    }

  }
}
