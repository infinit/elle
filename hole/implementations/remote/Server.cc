//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Server.cc
//
// created       julien quintard   [thu may 26 09:58:52 2011]
// updated       julien quintard   [thu aug 25 22:11:16 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Server.hh>
#include <hole/implementations/remote/Manifest.hh>

#include <hole/Hole.hh>

#include <Infinit.hh>

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
      /// default constructor.
      ///
      Server::Server(const nucleus::Network&			network):
	Peer(network),

	state(Server::StateUnknown),
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
// ---------- peer ------------------------------------------------------------
//

      ///
      /// initialize the server by waiting for incoming connections.
      ///
      elle::Status	Server::Initialize(const elle::Point&	point)
      {
	enter();

	//
	// register the messages.
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      const elle::Cipher
	      >
	    >				response(&Server::Response, this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      >
	    >				authenticated(&Server::Authenticated,
						      this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      const nucleus::Address,
	      const nucleus::Derivable<nucleus::Block>
	      >
	    >				push(&Server::Push, this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      const nucleus::Address,
	      const nucleus::Version,
	      nucleus::Derivable<nucleus::Block>
	      >
	    >				pull(&Server::Pull, this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      const nucleus::Address
	      >
	    >				wipe(&Server::Wipe, this);

	  // register the response message.
	  if (elle::Network::Register(
	        elle::Procedure<TagResponse>(response)) ==
	      elle::StatusError)
	    escape("unable to register the callback");

	  // register the push message.
	  if (elle::Network::Register(
	        elle::Procedure<TagPush,
				elle::TagOk>(push,
					     &authenticated,
					     NULL)) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the pull message.
	  if (elle::Network::Register(
	        elle::Procedure<TagPull,
				TagBlock>(pull,
					  &authenticated,
					  NULL)) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the wipe message.
	  if (elle::Network::Register(
	        elle::Procedure<TagWipe,
				elle::TagOk>(wipe,
					     &authenticated,
					     NULL)) == elle::StatusError)
	    escape("unable to register the callback");
	}

	//
	// create the connection.
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      elle::Gate*
	    >
	  >				connection(&Server::Connection, this);

	  // listen for incoming connections.
	  if (elle::Bridge::Listen(point,
				   connection) == elle::StatusError)
	    escape("unable to listen for bridge connections");
	}

	leave();
      }

      ///
      /// clean the server.
      ///
      elle::Status	Server::Clean()
      {
	elle::Point	point;

	enter();

	// retrieve the address.
	if (this->gate->Target(point) == elle::StatusError)
	  escape("unable to retrieve the target");

	// close the client connection, if necessary.
	if (this->gate != NULL)
	  {
	    // disconnect.
	    if (this->gate->Disconnect() == elle::StatusError)
	      escape("unable to disconnect the client gate");
	  }

	// block the incoming connections.
	if (elle::Bridge::Block(point) == elle::StatusError)
	  escape("unable to block bridge connections");

	leave();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Server::Put(const nucleus::Address&	address,
				    const nucleus::ImmutableBlock& block)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Put[Immutable]()"
		    << std::endl;

	// does the block already exist.
	if (block.Exist(this->network, address) == elle::StatusTrue)
	  escape("this immutable block seems to already exist");

	// store the block.
	if (block.Store(this->network, address) == elle::StatusError)
	  escape("unable to store the block");

	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Server::Put(const nucleus::Address&	address,
				    const nucleus::MutableBlock& block)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Put[Mutable]()"
		    << std::endl;

	// does the block already exist.
	if (block.Exist(this->network,
			address,
			nucleus::Version::Last) == elle::StatusTrue)
	  {
	    nucleus::MutableBlock*	current;

	    // build a block according to the component.
	    if (nucleus::Nucleus::Factory.Build(address.component,
						current) == elle::StatusError)
	      escape("unable to build the block");

	    // load the latest version.
	    if (current->Load(this->network,
			      address,
			      nucleus::Version::Last) == elle::StatusError)
	      escape("unable to load the current version");

	    // does the given block derive the current version.
	    if (!(block.version > current->version))
	      escape("the block to store does not seem to derive the current "
		     "version");

	    // delete the current instance.
	    delete current;

	    // waive.
	    waive(current);

	    // release.
	    release();
	  }

	// store the block.
	if (block.Store(this->network, address) == elle::StatusError)
	  escape("unable to store the block");

	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Server::Get(const nucleus::Address&	address,
				    nucleus::ImmutableBlock&	block)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Get[Immutable]()"
		    << std::endl;

	// does the block exist.
	if (block.Exist(this->network, address) == elle::StatusFalse)
	  escape("the block does not seem to exist");

	// load the block.
	if (block.Load(this->network, address) == elle::StatusError)
	  escape("unable to load the block");

	// validate the block.
	if (block.Validate(address) == elle::StatusError)
	  escape("the block seems to be invalid");

	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Server::Get(const nucleus::Address&	address,
				    const nucleus::Version&	version,
				    nucleus::MutableBlock&	block)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Get[Mutable]()"
		    << std::endl;

	// does the block exist.
	if (block.Exist(this->network, address, version) == elle::StatusFalse)
	  escape("the block does not seem to exist");

	// load the block.
	if (block.Load(this->network, address, version) == elle::StatusError)
	  escape("unable to load the block");

	// validate the block.
	if (block.Validate(address) == elle::StatusError)
	  escape("the block seems to be invalid");

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Server::Kill(const nucleus::Address&	address)
      {
	nucleus::Block	block;

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Kill()"
		    << std::endl;

	// treat the request depending on the nature of the block which
	// the addres indicates.
	switch (address.family)
	  {
	  case nucleus::FamilyContentHashBlock:
	    {
	      nucleus::ImmutableBlock	ib;

	      // erase the immutable block.
	      if (ib.Erase(this->network, address) == elle::StatusError)
		escape("unable to erase the block");

	      break;
	    }
	  case nucleus::FamilyPublicKeyBlock:
	  case nucleus::FamilyOwnerKeyBlock:
	  case nucleus::FamilyImprintBlock:
	    {
	      nucleus::MutableBlock	mb;

	      // retrieve the mutable block.
	      if (mb.Erase(this->network, address) == elle::StatusError)
		escape("unable to erase the block");

	      break;
	    }
	  default:
	    {
	      escape("unknown block family");
	    }
	  }

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback handles new connections.
      ///
      elle::Status	Server::Connection(elle::Gate*&		gate)
      {
	elle::Callback<
	  elle::Status,
	  elle::Parameters<>
	>				monitor(&Server::Monitor, this);

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Connection()"
		    << std::endl;

	// check if there already is a client.
	if (this->gate != NULL)
	  escape("a client seems to already be connected");

	// register the client.
	this->gate = gate;

	// register the error callback.
	if (this->gate->Monitor(monitor) == elle::StatusError)
	  escape("unable to monitor the connection");

	// set the state.
	this->state = Server::StateConnected;

	// send a message to the client in order to challenge it.
	if (this->gate->Send(
	      elle::Inputs<TagChallenge>()) == elle::StatusError)
	  escape("unable to send a message to the client");

	leave();
      }

      ///
      /// this callback is triggered whenever the client responds to
      /// the initial challenge.
      ///
      elle::Status	Server::Response(const elle::Cipher&	cipher)
      {
	elle::SecretKey	key;
	elle::String	string;
	elle::String	name;

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Response()"
		    << std::endl;

	// retrieve the key.
	if (Hole::Descriptor.Get("remote", "key",
				 string) == elle::StatusError)
	  escape("unable to retrieve the remote key from the "
		 "network descriptor");

	// create a key with the given string.
	if (key.Create(string) == elle::StatusError)
	  escape("unable to create the secret key");

	// decrypt the cipher.
	if (key.Decrypt(cipher, name) == elle::StatusError)
	  escape("unable to decrypt the cipher");

	// compare the network with the network being handled.
	if (Hole::Descriptor.name != name)
	  {
	    // disconnect.
	    if (this->gate->Disconnect() == elle::StatusError)
	      escape("unable to disconnect the client gate");

	    // delete the gate.
	    delete this->gate;

	    // reset the pointer.
	    this->gate = NULL;
	  }
	else
	  {
	    // set the state as authenticated.
	    this->state = Server::StateAuthenticated;
	  }

	leave();
      }

      ///
      /// this method returns an error if the client has not completed
      /// the authentication process.
      ///
      elle::Status	Server::Authenticated()
      {
	enter();

	// check that the client has been authenticated.
	if (this->state != Server::StateAuthenticated)
	  escape("the client has not been authenticated");

	leave();
      }

      ///
      /// this method stores the given block.
      ///
      elle::Status	Server::Push(const nucleus::Address&	address,
				     const
				       nucleus::Derivable
				         <nucleus::Block>&	derivable)
      {
	nucleus::Block*	object;

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Push()"
		    << std::endl;

	// infer the block from the derivable.
	if (derivable.Infer(object) == elle::StatusError)
	  escape("unable to infer the block from the derivable");

	// forward the request depending on the nature of the block which
	// the addres indicates.
	switch (address.family)
	  {
	  case nucleus::FamilyContentHashBlock:
	    {
	      nucleus::ImmutableBlock*	ib;

	      // cast to an immutable block.
	      ib = static_cast<nucleus::ImmutableBlock*>(object);

	      // store the immutable block.
	      if (this->Put(address, *ib) == elle::StatusError)
		escape("unable to put the block");

	      break;
	    }
	  case nucleus::FamilyPublicKeyBlock:
	  case nucleus::FamilyOwnerKeyBlock:
	  case nucleus::FamilyImprintBlock:
	    {
	      nucleus::MutableBlock*	mb;

	      // cast to a mutable block.
	      mb = static_cast<nucleus::MutableBlock*>(object);

	      // store the mutable block.
	      if (this->Put(address, *mb) == elle::StatusError)
		escape("unable to put the block");

	      break;
	    }
	  default:
	    {
	      escape("unknown block family");
	    }
	  }

	leave();
      }

      ///
      /// this method returns the block associated with the given address.
      ///
      elle::Status	Server::Pull(const nucleus::Address&	address,
				     const nucleus::Version&	version,
				     nucleus::Derivable<
				       nucleus::Block>&		derivable)
      {
	nucleus::Block*	block;

	enter(instance(block));

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Pull()"
		    << std::endl;

	// build the block according to the component.
	if (nucleus::Nucleus::Factory.Build(address.component,
					    block) == elle::StatusError)
	  escape("unable to build the block");

	// forward the request depending on the nature of the block which
	// the addres indicates.
	switch (address.family)
	  {
	  case nucleus::FamilyContentHashBlock:
	    {
	      nucleus::ImmutableBlock*	ib;

	      // cast to an immutable block.
	      ib = static_cast<nucleus::ImmutableBlock*>(block);

	      // retrieve the immutable block.
	      if (this->Get(address, *ib) == elle::StatusError)
		escape("unable to get the block");

	      break;
	    }
	  case nucleus::FamilyPublicKeyBlock:
	  case nucleus::FamilyOwnerKeyBlock:
	  case nucleus::FamilyImprintBlock:
	    {
	      nucleus::MutableBlock*	mb;

	      // cast to a mutable block.
	      mb = static_cast<nucleus::MutableBlock*>(block);

	      // retrieve the mutable block.
	      if (this->Get(address, version,
			    *mb) == elle::StatusError)
		escape("unable to get the block");

	      break;
	    }
	  default:
	    {
	      escape("unknown block family");
	    }
	  }

	// set the derivable and make the policy dynamic so that
	// the block gets deleted along with the derivable.
	derivable.product = address.component;
	derivable.object = block;
	derivable.policy = nucleus::Derivable<nucleus::Block>::PolicyDynamic;

	// waive the block.
	waive(block);

	leave();
      }

      ///
      /// this method removes the block associated with the given address.
      ///
      elle::Status	Server::Wipe(const nucleus::Address&	address)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Wipe()"
		    << std::endl;

	// forward the kill request to the implementation.
	if (this->Kill(address) == elle::StatusError)
	  escape("unable to erase the block");

	leave();
      }

      ///
      /// this callback is triggered whenever something unexpected occurs
      /// on the connection.
      ///
      elle::Status	Server::Monitor()
      {
	enter();

	if (this->gate->state == elle::Channel::StateDisconnected)
	  {
	    // delete the gate.
	    delete this->gate;

	    // set the gate to null.
	    this->gate = NULL;

	    // reset the state.
	    this->state = Server::StateUnknown;
	  }

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the server.
      ///
      elle::Status	Server::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Server]" << std::endl;

	// dump the parent.
	if (Peer::Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the peer");

	// dump the gate.
	if (this->gate != NULL)
	  {
	    if (this->gate->Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the gate");
	  }
	else
	  {
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Gate] " << elle::none
		      << std::endl;
	  }


	leave();
      }

    }
  }
}
