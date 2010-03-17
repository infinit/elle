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
// updated       julien quintard   [wed mar 17 10:36:24 2010]
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
    Status		Server::Setup(const String&		line)
    {
      enter();

      // set the line.
      this->line = line;

      leave();
    }

    ///
    /// this method is the thread entry point.
    ///
    Status		Server::Run()
    {
      Method<Door*>	connection(this, &Server::Connection);

      enter();

      std::cout << "[bridge] " << line << std::endl;

      // listen for incoming connections.
      if (Bridge::Listen(this->line, connection) == StatusError)
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
      String		challenge("CHALLENGE");
      String		response;

      enter();

      // push the door in the container in order not to lose the object.
      this->doors.push_front(door);

      std::cout << "[challenging...] " << std::endl;

      // call the challenge.
      if (door->Call(Inputs<TagChallenge>(challenge),
		     Outputs<TagResponse>(response)) == StatusError)
	escape("unable to call the challenge");

      std::cout << "[response] " << response << std::endl;

      leave();
    }

  }
}
