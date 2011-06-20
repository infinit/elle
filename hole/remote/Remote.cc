//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Remote.cc
//
// created       julien quintard   [fri may 20 19:32:16 2011]
// updated       julien quintard   [sun jun 19 18:05:31 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/remote/Remote.hh>

namespace hole
{
  namespace remote
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Remote::Remote(const nucleus::Network&			network):
      Holeable(network),

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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets the remote host's address.
    ///
    elle::Status	Remote::Host(const elle::Address&	host)
    {
      enter();

      // set the host.
      this->host = host;

      leave();
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
      Client*		client;
      Server*		server;

      enter(instance(client),
	    instance(server));

      // try to connect to the server's host.
      {
	// allocate a client.
	client = new Client(this->network, this->host);

	// initialize the client.
	if (client->Initialize() == elle::StatusOk)
	  {
	    // set the client as the node.
	    this->node = client;


	    // waive.
	    waive(client);

	    leave();
	  }
      }

      // purge the error messages.
      purge();

      // if the client did not succeed, create a server a wait for a client.
      {
	// allocate a server.
	server = new Server(this->network, this->host);

	// initialize the server.
	if (server->Initialize() == elle::StatusOk)
	  {
	    // set the server as the node.
	    this->node = server;

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

      // forward the request to the node.
      if (this->node->Kill(address) == elle::StatusError)
	escape("unable to kill the block");

      leave();
    }

  }
}
