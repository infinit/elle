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
// updated       julien quintard   [wed mar 10 20:01:59 2010]
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
    Status		Server::Run()
    {
      static Method<Door*>	connection(this, &Server::Connection);
      static Method<String>	response(this, &Server::Response);

      enter();

      std::cout << "[bridge] " << name << std::endl;

      // register the message.
      if (Network::Register<TagResponse>(response) == StatusError)
	escape("unable to register the response message");

      // listen for incoming connections.
      if (Bridge::Listen(this->name, connection) == StatusError)
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
      String		challenge("bande!");

      enter();

      // push the door in the container.
      this->doors.push_front(door);

      // send the challenge.
      if (door->Send(Inputs<TagChallenge>(challenge)) == StatusError)
	escape("unable to send the challenge");

      leave();
    }

    ///
    /// this method handles the response
    ///
    Status		Server::Response(String&		text)
    {
      enter();

      // simply display the text.
      std::cout << "[Response] " << text << std::endl;

      leave();
    }

  }
}
