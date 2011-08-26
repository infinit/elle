//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/cirkle/Cirkle.cc
//
// created       julien quintard   [fri aug 12 14:03:04 2011]
// updated       julien quintard   [fri aug 26 15:15:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/cirkle/Cirkle.hh>

#include <hole/Hole.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this value defines the component's name.
      ///
      const elle::Character		Component[] = "cirkle";

      ///
      /// XXX
      ///
      const elle::Natural16		Cirkle::Port = 4242;

      ///
      /// XXX
      ///
      Peer				Cirkle::Machine;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor.
      ///
      Cirkle::Cirkle(const nucleus::Network&			network):
	Holeable(network)
      {
      }

//
// ---------- holeable --------------------------------------------------------
//

      ///
      /// this method tries to connect to the server. if impossible, a server
      /// is created.
      ///
      elle::Status	Cirkle::Join()
      {
	elle::String	string;
	elle::Point	point;
	elle::Port	port;

	enter();

	// XXX improve this with getting a list of hosts.

	//
	// retrieve the parameters.
	//
	{
	  // retrieve the neighbours' points.
	  if (Hole::Descriptor.Get("cirkle", "neighbours",
				   string) == elle::StatusError)
	    escape("unable to retrieve the cirkle's host address from the "
		   "network descriptor");

	  // build the host point.
	  if (point.Create(string) == elle::StatusError)
	    escape("unable to create the host point");

	  // retrieve the peer's listening port.
	  if (Hole::Descriptor.Get("cirkle", "port",
				   port,
				   Cirkle::Port) == elle::StatusError)
	    escape("unable to retrieve the cirkle's local port from the "
		   "network descriptor");
	}

	// initialize the machine.
	if (Cirkle::Machine.Initialize(port, point) == elle::StatusError)
	  escape("unable to initialize the machine");

	leave();
      }

      ///
      /// this method cleans the peer.
      ///
      elle::Status	Cirkle::Leave()
      {
	enter();

	// clean the machine.
	if (Cirkle::Machine.Clean() == elle::StatusError)
	  escape("unable to clean the machine");

	leave();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Cirkle::Put(const nucleus::Address&	address,
				    const nucleus::ImmutableBlock& block)
      {
	enter();

	// forward the request to the peer.
	if (Cirkle::Machine.Put(address, block) == elle::StatusError)
	  escape("unable to put the block");

	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Cirkle::Put(const nucleus::Address&	address,
				    const nucleus::MutableBlock& block)
      {
	enter();

	// forward the request to the peer.
	if (Cirkle::Machine.Put(address, block) == elle::StatusError)
	  escape("unable to put the block");

	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Cirkle::Get(const nucleus::Address&	address,
				    nucleus::ImmutableBlock&	block)
      {
	enter();

	// forward the request to the peer.
	if (Cirkle::Machine.Get(address, block) == elle::StatusError)
	  escape("unable to get the block");

	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Cirkle::Get(const nucleus::Address&	address,
				    const nucleus::Version&	version,
				    nucleus::MutableBlock&	block)
      {
	enter();

	// forward the request to the peer.
	if (Cirkle::Machine.Get(address, version, block) == elle::StatusError)
	  escape("unable to get the block");

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Cirkle::Kill(const nucleus::Address&	address)
      {
	enter();

	// forward the request to the peer.
	if (Cirkle::Machine.Kill(address) == elle::StatusError)
	  escape("unable to kill the block");

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the implementation.
      ///
      elle::Status	Cirkle::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Cirkle]" << std::endl;

	// dump the parent.
	if (Holeable::Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the holeabl");

	leave();
      }

//
// ---------- static methods --------------------------------------------------
//

      ///
      /// this method dumps the static attributes.
      ///
      elle::Status	Cirkle::Show(const elle::Natural32	margin)
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Cirkle]" << std::endl;

	// dump the machine.
	if (Cirkle::Machine.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the machine");

	leave();
      }

    }
  }
}
