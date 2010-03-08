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
// updated       julien quintard   [fri mar  5 22:41:54 2010]
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
    /// this method is the thread entry point.
    ///
    void		Server::run()
    {
      static Method<Door*>	connection(this, &Server::Connection);

      enter();

      std::cout << "[bridge] " << name << std::endl;

      // listen for incoming connections.
      if (Bridge::Listen(this->name, connection) == StatusError)
	alert("unable to listen for bridge connections");

      // event loop.
      this->exec();

      release();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles new connections.
    ///
    Status		Server::Connection(Door*&		door)
    {
      String		challenge("bande!");
      String		response;

      enter();

      printf("Server::Connection\n");

      // push the door in the container.
      this->doors.push_front(door);

      // XXX
      door->Transmit(Inputs<TagChallenge>(challenge));
      //door->Transmit(Inputs<TagChallenge>(challenge));

      //door->Call(Inputs<TagChallenge>(challenge),
      //Outputs<TagResponse>(response));

      /*
      // send a message to the caller.
      if (door->Call(Inputs<TagChallenge>(challenge),
		     Outputs<TagResponse>(response)) == StatusError)
	{
	  // XXX
	  expose();

	escape("unable to send a message");
	}

      // print the response.
      std::cout << "[Response] " << response << std::endl;
      */

      leave();
    }

  }
}
