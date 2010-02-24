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
// updated       julien quintard   [tue feb 23 14:31:24 2010]
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
    /// this method initializes the server.
    ///
    Status		Client::Start(const String&		name)
    {
      static Method<Environment, String> receive(this, &Client::Receive);

      // register the echo message.
      if (Network::Register<TagEcho>(&receive) == StatusError)
	escape("unable to register the echo message");

      std::cout << "[bridge] " << name << std::endl;

      // create the door.
      if (this->door.Create() == StatusError)
	escape("unable to create the slot");

      // connect the door.
      if (this->door.Connect(name) == StatusError)
	escape("unable to connect to the bridge");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles echo messages.
    ///
    Status		Client::Receive(Environment&		environment,
					String&			text)
    {
      // simply display the text.
      std::cout << "[Echo] " << text << std::endl;

      leave();
    }

  }
}
