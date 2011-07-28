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
// updated       julien quintard   [tue jul 26 17:37:42 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Manifest.hh>

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
      /// default constructor.
      ///
      Client::Client(const nucleus::Network&			network,
		     const elle::Address&			host):
	Node(network, host),

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
// ---------- node ------------------------------------------------------------
//

      ///
      /// this method initializes the client.
      ///
      elle::Status	Client::Initialize()
      {
	enter();

	//
	// register the messages.
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      elle::Cipher
	      >
	    >				challenge(&Client::Challenge, this);

	  // register the challenge message.
	  if (elle::Network::Register(
	        elle::Procedure<TagChallenge,
				TagResponse>(challenge)) == elle::StatusError)
	    escape("unable to register the callback");
	}

	//
	// connect to the server.
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      const elle::String
	      >
	    >				error(&Client::Error, this);

	  // allocate the gate.
	  this->gate = new elle::Gate;

	  // create the gate.
	  if (this->gate->Create() == elle::StatusError)
	    escape("unable to create the gate");

	  // connect the gate.
	  if (this->gate->Connect(this->host) == elle::StatusError)
	    escape("unable to connect to the bridge");

	  // register the error callback.
	  if (this->gate->Monitor(error) == elle::StatusError)
	    escape("unable to monitor the connection");
	}

	leave();
      }

      ///
      /// this method cleans the client.
      ///
      /// the components are recycled just to make sure the memory is
      /// released before the Meta allocator terminates.
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
      elle::Status	Client::Challenge(elle::Cipher&		cipher)
      {
	elle::SecretKey	key;
	elle::String	string;

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

	leave();
      }

      ///
      /// this callback is triggered whenever the connection is shut down.
      ///
      elle::Status	Client::Error(const elle::String&)
      {
	enter();

	// delete the gate.
	delete this->gate;

	// reset the gate.
	this->gate = NULL;

	leave();
      }

      ///
      /// this method dumps the client.
      ///
      elle::Status	Client::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Client]" << std::endl;

	// dump the parent.
	if (Node::Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the node");

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
