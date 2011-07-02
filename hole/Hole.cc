//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Hole.cc
//
// created       julien quintard   [tue apr 13 15:27:20 2010]
// updated       julien quintard   [fri jul  1 14:01:07 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Hole.hh>

#include <Infinit.hh>

namespace hole
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "hole";

  ///
  /// this variable contains the network descriptor.
  ///
  lune::Descriptor		Hole::Descriptor;

  ///
  /// this variable holds the hole implementation.
  ///
  Holeable*			Hole::Implementation = NULL;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the hole by allocating and initializing
  /// the implementation.
  ///
  elle::Status		Hole::Initialize()
  {
    nucleus::Network	network;
    elle::String	name;

    enter();

    // retrieve the network name.
    if (Infinit::Parser->Value("Network", name) == elle::StatusError)
      escape("unable to retrieve the network name");

    // does the network exist.
    if (Hole::Descriptor.Exist(name) == elle::StatusFalse)
      escape("this network does not seem to exist");

    // load the descriptor.
    if (Hole::Descriptor.Load(name) == elle::StatusError)
      escape("unable to load the descriptor");

    // pull the attributes.
    if (Hole::Descriptor.Pull() == elle::StatusError)
      escape("unable to pull the descriptor's attributes");

    // validate the descriptor.
    if (Hole::Descriptor.Validate(Infinit::Authority) != elle::StatusTrue)
      escape("unable to validate the descriptor");

    // create the network instance.
    if (network.Create(name) == elle::StatusError)
      escape("unable to create the network instance");

    // create the holeable depending on the model.
    switch (Hole::Descriptor.model.type)
      {
      case Model::TypeLocal:
	{
	  // allocate the instance.
	  Hole::Implementation = new local::Local(network);

	  break;
	}
      case Model::TypeRemote:
	{
	  /* XXX
	  remote::Remote*        remote;

	  // allocate the instance.
	  remote = new remote::Remote(network);

	  // set remote host.
	  if (remote->Host(boot) == elle::StatusError)
	    escape("unable to set the host");

	  // set the implementation.
	  Hole::Implementation = remote;
	  */

	  break;
	}
      default:
	escape("unknown or not-yet-supported model");
      }

    // set the root address.
    if (Hole::Implementation->Root(Hole::Descriptor.root) == elle::StatusError)
      escape("unable to set the root address");

    leave();
  }

  ///
  /// this method cleans the hole.
  ///
  elle::Status		Hole::Clean()
  {
    enter();

    // nothing to do.

    leave();
  }

  ///
  /// this method sets up the hole-specific options.
  ///
  elle::Status		Hole::Options()
  {
    enter();

    // register the option.
    if (Infinit::Parser->Register(
	  "Network",
	  'n',
	  "network",
	  "specifies the name of the network",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    leave();
  }

  ///
  /// this method returns the address of the root block i.e the origin.
  ///
  elle::Status		Hole::Origin(nucleus::Address&		address)
  {
    enter();

    printf("[XXX] Hole::Origin()\n");

    // return the address.
    address = Hole::Implementation->root;

    leave();
  }

  ///
  /// this method stores the given block.
  ///
  elle::Status		Hole::Push(const nucleus::Address&	address,
				   const nucleus::Block&	block)
  {
    enter();

    printf("[XXX] Hole::Push()\n");

    // forward the request depending on the nature of the block which
    // the address indicates.
    switch (address.family)
      {
      case nucleus::FamilyContentHashBlock:
	{
	  const nucleus::ImmutableBlock*	ib;

	  // cast to an immutable block.
	  ib = static_cast<const nucleus::ImmutableBlock*>(&block);

	  // store the immutable block.
	  if (Hole::Implementation->Put(address, *ib) == elle::StatusError)
	    escape("unable to put the block");

	  break;
	}
      case nucleus::FamilyPublicKeyBlock:
      case nucleus::FamilyOwnerKeyBlock:
      case nucleus::FamilyImprintBlock:
	{
	  const nucleus::MutableBlock*		mb;

	  // cast to a mutable block.
	  mb = static_cast<const nucleus::MutableBlock*>(&block);

	  // store the mutable block.
	  if (Hole::Implementation->Put(address, *mb) == elle::StatusError)
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
  elle::Status		Hole::Pull(const nucleus::Address&	address,
				   const nucleus::Version&	version,
				   nucleus::Block&		block)
  {
    enter();

    printf("[XXX] Hole::Pull()\n");

    // forward the request depending on the nature of the block which
    // the addres indicates.
    switch (address.family)
      {
      case nucleus::FamilyContentHashBlock:
	{
	  nucleus::ImmutableBlock*	ib;

	  // cast to an immutable block.
	  ib = static_cast<nucleus::ImmutableBlock*>(&block);

	  // retrieve the immutable block.
	  if (Hole::Implementation->Get(address, *ib) == elle::StatusError)
	    escape("unable to get the block");

	  break;
	}
      case nucleus::FamilyPublicKeyBlock:
      case nucleus::FamilyOwnerKeyBlock:
      case nucleus::FamilyImprintBlock:
	{
	  nucleus::MutableBlock*	mb;

	  // cast to a mutable block.
	  mb = static_cast<nucleus::MutableBlock*>(&block);

	  // retrieve the mutable block.
	  if (Hole::Implementation->Get(address, version,
					*mb) == elle::StatusError)
	    escape("unable to get the block");

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
  /// this method removes the block associated with the given address.
  ///
  elle::Status		Hole::Wipe(const nucleus::Address&	address)
  {
    enter();

    printf("[XXX] Hole::Wipe()\n");

    // forward the kill request to the implementation.
    if (Hole::Implementation->Kill(address) == elle::StatusError)
      escape("unable to erase the block");

    leave();
  }

}
