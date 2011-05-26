//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Client.cc
//
// created       julien quintard   [thu may 26 10:22:03 2011]
// updated       julien quintard   [thu may 26 14:50:30 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/remote/Client.hh>
#include <hole/remote/Manifest.hh>

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
    Client::Client(const nucleus::Network&			network,
		   const elle::Address&				host):
      Node(network, host),

      gate(elle::Socket::ModeAsynchronous)
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
      elle::Callback<const elle::String>	error(&Client::Error, this);

      enter();

      // create the gate.
      if (this->gate.Create() == elle::StatusError)
	escape("unable to create the gate");

      // register the error callback.
      if (this->gate.Monitor(error) == elle::StatusError)
	escape("unable to monitor the connection");

      // connect the gate.
      if (this->gate.Connect(this->host) == elle::StatusError)
	escape("unable to connect to the bridge");

      leave();
    }

    ///
    /// this method cleans the client.
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
      elle::Derivable<nucleus::Block>	derivable(address.component, block);

      enter();

      // transfer to the remote.
      if (this->gate.Call(
	    elle::Inputs<TagRemotePush>(this->network,
					address,
					derivable),
	    elle::Outputs<TagRemoteOk>()) == elle::StatusError)
	escape("unable to transfer the request");

      leave();
    }

    ///
    /// this method stores a mutable block.
    ///
    elle::Status	Client::Put(const nucleus::Address&	address,
				    const nucleus::MutableBlock& block)
    {
      elle::Derivable<nucleus::Block>	derivable(address.component, block);

      enter();

      // transfer to the remote.
      if (this->gate.Call(
	    elle::Inputs<TagRemotePush>(this->network,
					address,
					derivable),
	    elle::Outputs<TagRemoteOk>()) == elle::StatusError)
	escape("unable to transfer the request");

      leave();
    }

    ///
    /// this method retrieves an immutable block.
    ///
    elle::Status	Client::Get(const nucleus::Address&	address,
				    nucleus::ImmutableBlock&	block)
    {
      elle::Derivable<nucleus::Block>	derivable(block);

      enter();

      // transfer to the remote.
      if (this->gate.Call(
	    elle::Inputs<TagRemotePull>(this->network,
					address,
					nucleus::Version::Any),
	    elle::Outputs<TagRemoteBlock>(derivable)) == elle::StatusError)
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
      elle::Derivable<nucleus::Block>	derivable(block);

      enter();

      // transfer to the remote.
      if (this->gate.Call(
	    elle::Inputs<TagRemotePull>(this->network,
					address,
					version),
	    elle::Outputs<TagRemoteBlock>(derivable)) == elle::StatusError)
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
	    elle::Inputs<TagRemoteWipe>(this->network,
					address),
	    elle::Outputs<TagRemoteOk>()) == elle::StatusError)
	escape("unable to transfer the request");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever the connection is shut down.
    ///
    elle::Status	Client::Error(const elle::String&	error)
    {
      enter();

      // quit the program since the server has probably existed as well.
      if (elle::Program::Exit() == elle::StatusError)
	escape("unable to exit the program");

      leave();
    }

  }
}
