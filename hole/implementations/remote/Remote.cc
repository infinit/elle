//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Remote.cc
//
// created       julien quintard   [fri may 20 19:32:16 2011]
// updated       julien quintard   [sun aug 28 21:36:32 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Remote.hh>

#include <hole/Hole.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this value defines the component's name.
      ///
      const elle::Character		Component[] = "remote";

      ///
      /// XXX
      ///
      Remote*				Remote::Machine = NULL;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor.
      ///
      Remote::Remote(const nucleus::Network&			network):
	Holeable(network),

	role(Remote::RoleUnknown),
	peer(NULL)
      {
      }

      ///
      /// the destructor.
      ///
      Remote::~Remote()
      {
	// if a peer is present.
	if (this->peer != NULL)
	  delete this->peer;
      }

//
// ---------- holeable --------------------------------------------------------
//

      ///
      /// this method tries to connect to the server. if impossible, a server
      /// is created.
      ///
      elle::Status	Remote::Join()
      {
	elle::String	string;
	elle::Point	point;

	enter();

	// set the static variable.
	Remote::Machine = this;

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
	  client = new Client(this->network);

	  // initialize the client.
	  if (client->Initialize(point) == elle::StatusOk)
	    {
	      // set the client as the host.
	      this->peer = client;

	      // set the role.
	      this->role = Remote::RoleClient;

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
	  server = new Server(this->network);

	  // initialize the server.
	  if (server->Initialize(point) == elle::StatusOk)
	    {
	      // set the server as the host.
	      this->peer = server;

	      // set the role.
	      this->role = Remote::RoleServer;

	      // waive.
	      waive(server);

	      leave();
	    }

	  // delete the server.
	  delete server;
	}

	escape("unable to create a client or a server");
      }

      ///
      /// this method cleans the peer.
      ///
      elle::Status	Remote::Leave()
      {
	enter();

	// clean the peer.
	if (this->peer->Clean() == elle::StatusError)
	  escape("unable to clean the peer");

	leave();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Remote::Put(const nucleus::Address&	address,
				    const nucleus::ImmutableBlock& block)
      {
	enter();

	// check if this peer is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the peer.
	if (this->peer->Put(address, block) == elle::StatusError)
	  escape("unable to put the block");

	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Remote::Put(const nucleus::Address&	address,
				    const nucleus::MutableBlock& block)
      {
	enter();

	// check if this peer is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the peer.
	if (this->peer->Put(address, block) == elle::StatusError)
	  escape("unable to put the block");

	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Remote::Get(const nucleus::Address&	address,
				    nucleus::ImmutableBlock&	block)
      {
	enter();

	// check if this peer is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the peer.
	if (this->peer->Get(address, block) == elle::StatusError)
	  escape("unable to get the block");

	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Remote::Get(const nucleus::Address&	address,
				    const nucleus::Version&	version,
				    nucleus::MutableBlock&	block)
      {
	enter();

	// check if this peer is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the peer.
	if (this->peer->Get(address, version, block) == elle::StatusError)
	  escape("unable to get the block");

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Remote::Kill(const nucleus::Address&	address)
      {
	enter();

	// check if this peer is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the peer.
	if (this->peer->Kill(address) == elle::StatusError)
	  escape("unable to kill the block");

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the implementation.
      ///
      elle::Status	Remote::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Remote]" << std::endl;

	// dump the parent.
	if (Holeable::Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the holeabl");

	std::cout << alignment << elle::Dumpable::Shift
		  << "[Role] " << (elle::Natural32)this->role
		  << std::endl;

	// dump the peer.
	if (this->peer != NULL)
	  {
	    if (this->peer->Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the peer");
	  }
	else
	  {
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Peer] " << elle::none
		      << std::endl;
	  }

	leave();
      }

    }
  }
}
