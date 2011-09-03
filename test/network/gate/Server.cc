//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/gate/Server.cc
//
// created       julien quintard   [fri nov 27 22:04:36 2009]
// updated       julien quintard   [fri sep  2 20:44:52 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/gate/Server.hh>

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
    Server::Server():
      gate(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Server::~Server()
    {
      // if present, delete the gate.
      if (this->gate != NULL)
	delete this->gate;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the server.
    ///
    Status		Server::Setup(const String&		line)
    {
      enter();

      // set the point.
      if (this->point.Create(line) == StatusError)
	escape("unable to create the point");

      leave();
    }

    ///
    /// this method is the thread entry point.
    ///
    Status		Server::Run()
    {
      enter();

      std::cout << "[bridge]" << std::endl;
      point.Dump();

      // listen for incoming connections.
      if (Bridge::Listen(point,
			 Callback<>::Infer(&Server::Connection,
					   this)) == StatusError)
	escape("unable to listen for bridge connections");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles new connections.
    ///
    Status		Server::Connection(Gate*		gate)
    {
      String		challenge("CHALLENGE");
      String		response;

      enter();

      // if there is already a client, exit.
      if (this->gate != NULL)
	escape("there is already a client connected");

      // add the gate.
      this->gate = gate;

      std::cout << "[challenging...] " << std::endl;

      // call the challenge.
      if (gate->Call(Inputs<TagChallenge>(challenge),
		     Outputs<TagResponse>(response)) == StatusError)
	escape("unable to call the challenge");

      std::cout << "[response] " << response << std::endl;

      leave();
    }

  }
}
