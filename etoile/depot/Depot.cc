//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Depot.cc
//
// created       julien quintard   [tue sep  1 01:11:07 2009]
// updated       julien quintard   [mon may 23 13:56:40 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Depot.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the depot.
    ///
    elle::Status	Depot::Initialize()
    {
      enter();

      // initialize the repository.
      if (Repository::Initialize() == elle::StatusError)
	escape("unable to initialize the repository");

      // initialize the hole.
      if (Hole::Initialize() == elle::StatusError)
	escape("unable to initialize the hole");

      leave();
    }

    ///
    /// this method cleans the depot.
    ///
    elle::Status	Depot::Clean()
    {
      enter();

      // clean the hole.
      if (Hole::Clean() == elle::StatusError)
	escape("unable to clean the hole");

      // clean the repository.
      if (Repository::Clean() == elle::StatusError)
	escape("unable to clean the repository");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Depot::Origin(const nucleus::Network&	network,
				      nucleus::Address&		address)
    {
      enter();

      // XXX

      // request the hole.
      if (Hole::Origin(network, address) == elle::StatusError)
	escape("unable to retrieve the origin");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Depot::Push(const nucleus::Network&	network,
				    const nucleus::Address&	address,
				    const nucleus::Block&	block)
    {
      enter();

      // store in the hole.
      if (Hole::Push(network, address, block) == elle::StatusError)
	escape("unable to put the block in the hole");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Depot::Pull(const nucleus::Network&	network,
				    const nucleus::Address&	address,
				    const nucleus::Version&	version,
				    nucleus::Block&		block)
    {
      enter();

      // XXX look in the cache etc.

      // finally, look in the hole.
      if (Hole::Pull(network, address, version,
		     block) == elle::StatusError)
	escape("unable to retrieve the block from the hole");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Depot::Wipe(const nucleus::Network&	network,
				    const nucleus::Address&	address)
    {
      enter();

      // XXX look in the cache etc.

      // finally, erase the block from the hole.
      if (Hole::Wipe(network, address) == elle::StatusError)
	escape("unable to erase the block from the hole");

      leave();
    }

  }
}
