//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Client.cc
//
// created       julien quintard   [thu may 26 10:22:03 2011]
// updated       julien quintard   [sun aug 28 22:40:50 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Client.hh>
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
      Client::Client(const nucleus::Network&			network):
	Peer(network),

	gate(NULL)
      {
      }

      ///
      /// the destructor.
      ///
      Client::~Client()
      {
	// delete the gate.
	if (this->gate != NULL)
	  delete this->gate;
      }

//
// ---------- peer ------------------------------------------------------------
//

      ///
      /// this method initializes the client.
      ///
      elle::Status	Client::Initialize(const elle::Point&	point)
      {
	enter();

	//
	// register the messages.
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<>
	    >				challenge(&Client::Challenge, this);

	  // register the challenge message.
	  if (elle::Network::Register(
	        elle::Procedure<TagChallenge>(challenge)) == elle::StatusError)
	    escape("unable to register the callback");
	}

	//
	// connect to the server.
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<>
	    >				monitor(&Client::Monitor, this);

	  // allocate the gate.
	  this->gate = new elle::Gate;

	  // register the error callback.
	  if (this->gate->Monitor(monitor) == elle::StatusError)
	    escape("unable to monitor the connection");

	  // create the gate.
	  if (this->gate->Create() == elle::StatusError)
	    escape("unable to create the gate");

	  // connect the gate.
	  if (this->gate->Connect(point,
				  elle::Channel::ModeSynchronous) ==
	      elle::StatusError)
	    escape("unable to connect to the bridge");
	}

	leave();
      }

      ///
      /// this method cleans the client.
      ///
      elle::Status	Client::Clean()
      {
	enter();

	// disconnect.
	if (this->gate != NULL)
	  {
	    if (this->gate->Disconnect() == elle::StatusError)
	      escape("unable to transfer the request");
	  }

	leave();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Client::Put(const nucleus::Address&	address,
				    const nucleus::ImmutableBlock& block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(address.component,
							  block);

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Put[Immutable]()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Client::Put(const nucleus::Address&	address,
				    const nucleus::MutableBlock& block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(address.component,
							  block);

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Put[Mutable]()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Client::Get(const nucleus::Address&	address,
				    nucleus::ImmutableBlock&	block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(block);

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Get[Immutable]()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPull>(address,
				    nucleus::Version::Any),
	      elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Client::Get(const nucleus::Address&	address,
				    const nucleus::Version&	version,
				    nucleus::MutableBlock&	block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(block);

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Get[Mutable]()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPull>(address,
				    version),
	      elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Client::Kill(const nucleus::Address&	address)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Kill()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagWipe>(address),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback is triggered whenever the client is challenged
      /// by the server.
      ///
      /// the challenge consists in responding to the server with the
      /// network's name it is supposed to support, this being encrypted
      /// with the remote key provided in the network descriptor.
      ///
      elle::Status	Client::Challenge()
      {
	elle::SecretKey	key;
	elle::String	string;
	elle::Cipher	cipher;

	enter();

	// retrieve the key.
	if (Hole::Descriptor.Get("remote", "key",
				 string) == elle::StatusError)
	  escape("unable to retrieve the remote key from the "
		 "network descriptor");

	// create a key with the given string.
	if (key.Create(string) == elle::StatusError)
	  escape("unable to create the secret key");

	// encrypt the network's name with the key.
	if (key.Encrypt(Hole::Descriptor.name, cipher) == elle::StatusError)
	  escape("unable to encrypt the network's name");

	// return the cipher.
	if (this->gate->Reply(
	      elle::Inputs<TagResponse>(cipher)) == elle::StatusError)
	  escape("unable to return the challenge response");

	leave();
      }

      ///
      /// this callback is triggered whenever something unexpected
      /// occurs on the connection.
      ///
      elle::Status	Client::Monitor()
      {
	enter();

	if (this->gate->state == elle::Channel::StateDisconnected)
	  {
	    // delete the gate.
	    delete this->gate;

	    // reset the gate.
	    this->gate = NULL;
	  }

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the client.
      ///
      elle::Status	Client::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Client]" << std::endl;

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
		      << "[Gate] " << elle::none << std::endl;
	  }

	leave();
      }

    }
  }
}
