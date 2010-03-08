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
// updated       julien quintard   [fri mar  5 22:42:36 2010]
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
    void		Client::run()
    {
      static Method<String> challenge(this, &Client::Challenge);

      enter();

      // register the message.
      if (Network::Register<TagChallenge>(challenge) == StatusError)
	alert("unable to register the challenge message");

      std::cout << "[bridge] " << this->name << std::endl;

      // create the door.
      if (this->door.Create() == StatusError)
	alert("unable to create the slot");

      // connect the door.
      if (this->door.Connect(this->name) == StatusError)
	alert("unable to connect to the bridge");

      // event loop.
      this->exec();

      release();
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

      // XXX reply
      //this->door.Transmit(Inputs<TagResponse>(text));

      leave();
    }

  }
}
