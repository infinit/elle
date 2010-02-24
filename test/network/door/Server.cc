//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/door/Server.cc
//
// created       julien quintard   [fri nov 27 22:04:36 2009]
// updated       julien quintard   [tue feb 23 14:29:55 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/door/Server.hh>

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
    Status		Server::Start(const String&		name)
    {
      static Method<Door*>	connection(this, &Server::Connection);

      std::cout << "[bridge] " << name << std::endl;

      // listen for incoming connections.
      if (Bridge::Listen(name, &connection) == StatusError)
	escape("unable to listen for bridge connections");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles new connections.
    ///
    Status		Server::Connection(Door*&		door)
    {
      // send a message to the caller.
      if (door->Send<TagEcho>(String("bande!")) == StatusError)
	escape("unable to send a message");

      // disconnect right away.
      if (door->Disconnect() == StatusError)
	escape("unable to disconnect");

      // delete the door.
      delete door;

      leave();
    }

  }
}
