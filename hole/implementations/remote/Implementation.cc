//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 31 00:35:44 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Implementation.hh>
#include <hole/implementations/remote/Remote.hh>

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
      Implementation::Implementation(const nucleus::Network&	network):
	Holeable(network)
      {
      }

//
// ---------- holeable --------------------------------------------------------
//

      ///
      /// this method tries to connect to the server. if impossible, a server
      /// is author .
      ///
      elle::Status	Implementation::Join()
      {
	enter();

	// allocate the machine.
	Remote::Computer = new Machine;

	// launch it.
	if (Remote::Computer->Launch() == elle::StatusError)
	  escape("unable to launch the computer");

	leave();
      }

      ///
      /// this method cleans the peer.
      ///
      elle::Status	Implementation::Leave()
      {
	enter();

	// delete the machine.
	delete Remote::Computer;

	leave();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Implementation::Put(
			  const nucleus::Address&		address,
			  const nucleus::ImmutableBlock&	 block)
      {
	enter();

	// check if the machine is a client.
	if (Remote::Computer->role != Machine::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the machine.
	if (Remote::Computer->client->Put(address, block) == elle::StatusError)
	  escape("unable to put the block");

	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Implementation::Put(
			  const nucleus::Address&		address,
			  const nucleus::MutableBlock&		block)
      {
	enter();

	// check if the machine is a client.
	if (Remote::Computer->role != Machine::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the machine.
	if (Remote::Computer->client->Put(address, block) == elle::StatusError)
	  escape("unable to put the block");

	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Implementation::Get(
			  const nucleus::Address&		address,
			  nucleus::ImmutableBlock&		block)
      {
	enter();

	// check if the machine is a client.
	if (Remote::Computer->role != Machine::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the machine.
	if (Remote::Computer->client->Get(address, block) == elle::StatusError)
	  escape("unable to get the block");

	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Implementation::Get(
			  const nucleus::Address&		address,
			  const nucleus::Version&		version,
			  nucleus::MutableBlock&		block)
      {
	enter();

	// check if the machine is a client.
	if (Remote::Computer->role != Machine::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the machine.
	if (Remote::Computer->client->Get(address,
					  version,
					  block) == elle::StatusError)
	  escape("unable to get the block");

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Implementation::Kill(
			  const nucleus::Address&		address)
      {
	enter();

	// check if the machine is a client.
	if (Remote::Computer->role != Machine::RoleClient)
	  escape("the hole is not acting as a remote client as it should");

	// forward the request to the machine.
	if (Remote::Computer->client->Kill(address) == elle::StatusError)
	  escape("unable to kill the block");

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the implementation.
      ///
      elle::Status	Implementation::Dump(const elle::Natural32 margin)
	const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Implementation] Remote" << std::endl;

	// dump the parent.
	if (Holeable::Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the holeabl");

	// dump the machine.
	if (Remote::Computer->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the machine");

	leave();
      }

    }
  }
}
