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
// updated       julien quintard   [thu jul 14 14:21:19 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Manifest.hh>

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
	Node(network, host)
      {
      }

//
// ---------- node ------------------------------------------------------------
//

      ///
      /// this method initializes the client.
      ///
      elle::Status	Client::Initialize()
      {
	elle::Callback< elle::Parameters<const elle::String> >
	  error(&Client::Error, this);

	enter();

	// create the gate.
	if (this->gate.Create() == elle::StatusError)
	  escape("unable to create the gate");

	// connect the gate.
	if (this->gate.Connect(this->host) == elle::StatusError)
	  escape("unable to connect to the bridge");

	// register the error callback.
	if (this->gate.Monitor(error) == elle::StatusError)
	  escape("unable to monitor the connection");

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
	if (this->gate.Disconnect() == elle::StatusError)
	  escape("unable to transfer the request");

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

	// transfer to the remote.
	if (this->gate.Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<TagOk>()) == elle::StatusError)
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

	// transfer to the remote.
	if (this->gate.Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<TagOk>()) == elle::StatusError)
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

	// transfer to the remote.
	if (this->gate.Call(
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

	// transfer to the remote.
	if (this->gate.Call(
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

	// transfer to the remote.
	if (this->gate.Call(
	      elle::Inputs<TagWipe>(address),
	      elle::Outputs<TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback is triggered whenever the connection is shut down.
      ///
      elle::Status	Client::Error(const elle::String&)
      {
	enter();

	// XXX log or display error.

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
	if (this->gate.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the gate");

	leave();
      }

    }
  }
}
