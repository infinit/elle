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
// updated       julien quintard   [fri jul  8 12:57:16 2011]
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

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor.
      ///
      Remote::Remote(const nucleus::Network&			network):
	Holeable(network),

	role(Remote::RoleUnknown),
	node(NULL)
      {
      }

      ///
      /// the destructor.
      ///
      Remote::~Remote()
      {
	// if a node is present.
	if (this->node != NULL)
	  delete this->node;
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
	elle::Address	address;

	enter();

	// retrieve the server's address.
	if (Hole::Descriptor.Get("remote", "host",
				 string) == elle::StatusError)
	  escape("unable to retrieve the remote's host address from the "
		 "network descriptor");

	// build the host address.
	if (this->host.Create(string) == elle::StatusError)
	  escape("unable to create the host address");

	// try to connect to the server's host.
	{
	  Client*		client;

	  enter(instance(client));

	  // allocate a client.
	  client = new Client(this->network, this->host);

	  // initialize the client.
	  if (client->Initialize() == elle::StatusOk)
	    {
	      // set the client as the node.
	      this->node = client;

	      // set the role.
	      this->role = Remote::RoleClient;

	      // waive.
	      waive(client);

	      leave();
	    }
	}

	// purge the error messages.
	purge();

	// if the client did not succeed, create a server a wait for a client.
	{
	  Server*		server;

	  enter(instance(server));

	  // allocate a server.
	  server = new Server(this->network, this->host);

	  // initialize the server.
	  if (server->Initialize() == elle::StatusOk)
	    {
	      // set the server as the node.
	      this->node = server;

	      // set the role.
	      this->role = Remote::RoleServer;

	      // waive.
	      waive(server);

	      leave();
	    }
	}

	escape("unable to create a client or a server");
      }

      ///
      /// this method cleans the node.
      ///
      elle::Status	Remote::Leave()
      {
	enter();

	// clean the node.
	if (this->node->Clean() == elle::StatusError)
	  escape("unable to clean the node");

	leave();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Remote::Put(const nucleus::Address&	address,
				    const nucleus::ImmutableBlock& block)
      {
	enter();

	// check if this node is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the node.
	if (this->node->Put(address, block) == elle::StatusError)
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

	// check if this node is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the node.
	if (this->node->Put(address, block) == elle::StatusError)
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

	// check if this node is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the node.
	if (this->node->Get(address, block) == elle::StatusError)
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

	// check if this node is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the node.
	if (this->node->Get(address, version, block) == elle::StatusError)
	  escape("unable to get the block");

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Remote::Kill(const nucleus::Address&	address)
      {
	enter();

	// check if this node is a client.
	if (this->role != Remote::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the node.
	if (this->node->Kill(address) == elle::StatusError)
	  escape("unable to kill the block");

	leave();
      }

    }
  }
}
