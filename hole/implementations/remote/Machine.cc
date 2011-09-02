//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Machine.cc
//
// created       julien quintard   [wed aug 31 01:02:08 2011]
// updated       julien quintard   [fri sep  2 13:04:06 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Machine.hh>

#include <hole/Hole.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Machine::Machine():
	role(Machine::RoleUnknown)
      {
      }

      ///
      /// XXX
      ///
      Machine::~Machine()
      {
	// delete the entity depending on the role.
	switch (this->role)
	  {
	  case Machine::RoleServer:
	    {
	      delete this->server;

	      break;
	    }
	  case Machine::RoleClient:
	    {
	      delete this->client;

	      break;
	    }
	  case Machine::RoleUnknown:
	    {
	      // nothing to do.

	      break;
	    }
	  }
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status	Machine::Launch()
      {
	elle::String	string;
	elle::Point	point;

	enter();

	// retrieve the server's point.
	if (Hole::Descriptor.Get("remote", "server",
				 string) == elle::StatusError)
	  escape("unable to retrieve the server's point from the "
		 "network descriptor");

	// build the host point.
	if (point.Create(string) == elle::StatusError)
	  escape("unable to create the host point");

	// try to connect to the server's host.
	{
	  Client*		client;

	  enter(instance(client));

	  // allocate a client.
	  client = new Client(point);

	  // launch the client.
	  if (client->Launch() == elle::StatusOk)
	    {
	      // set the client as the host.
	      this->client = client;

	      // set the role.
	      this->role = Machine::RoleClient;

	      // waive.
	      waive(client);

	      leave();
	    }

	  // delete the client.
	  delete client;
	}

	// purge the error messages.
	purge();

	// if the client did not succeed, create a server a wait for a client.
	{
	  Server*		server;

	  enter(instance(server));

	  // allocate a server.
	  server = new Server(point);

	  // launch the server.
	  if (server->Launch() == elle::StatusOk)
	    {
	      // set the server as the host.
	      this->server = server;

	      // set the role.
	      this->role = Machine::RoleServer;

	      // waive.
	      waive(server);

	      leave();
	    }

	  // delete the server.
	  delete server;
	}

	escape("unable to create a client or a server");
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the machine.
      ///
      elle::Status	Machine::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Machine]" << std::endl;

	// dump the role.
	std::cout << alignment << elle::Dumpable::Shift
		  << "[Role] " << this->role << std::endl;

	// dump the entity.
	switch (this->role)
	  {
	  case Machine::RoleServer:
	    {
	      // dump the server.
	      if (this->server->Dump(margin + 2) == elle::StatusError)
		escape("unable to dump the server");

	      break;
	    }
	  case Machine::RoleClient:
	    {
	      // dump the client.
	      if (this->client->Dump(margin + 2) == elle::StatusError)
		escape("unable to dump the client");

	      break;
	    }
	  default:
	    {
	      // nothing to do.

	      break;
	    }
	  }

	leave();
      }

    }
  }
}
