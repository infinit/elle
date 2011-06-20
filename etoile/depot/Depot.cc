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
// updated       julien quintard   [tue jun 14 22:03:59 2011]
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

      /* XXX
      // initialize the repository.
      if (Repository::Initialize() == elle::StatusError)
	escape("unable to initialize the repository");
      */

      leave();
    }

    ///
    /// this method cleans the depot.
    ///
    elle::Status	Depot::Clean()
    {
      enter();

      /* XXX
      // clean the repository.
      if (Repository::Clean() == elle::StatusError)
	escape("unable to clean the repository");
      */

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Depot::Origin(nucleus::Address&		address)
    {
      enter();

      // XXX

      // request the hole.
      if (Hole::Origin(address) == elle::StatusError)
	escape("unable to retrieve the origin");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Depot::Push(const nucleus::Address&	address,
				    const nucleus::Block&	block)
    {
      enter();

      // store in the hole.
      if (Hole::Push(address, block) == elle::StatusError)
	escape("unable to put the block in the hole");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Depot::Pull(const nucleus::Address&	address,
				    const nucleus::Version&	version,
				    nucleus::Block&		block)
    {
      enter();

      // XXX look in the cache etc.

      // finally, look in the hole.
      if (Hole::Pull(address, version,
		     block) == elle::StatusError)
	escape("unable to retrieve the block from the hole");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Depot::Wipe(const nucleus::Address&	address)
    {
      enter();

      // XXX remove from the cache as well.

      // finally, erase the block from the hole.
      if (Hole::Wipe(address) == elle::StatusError)
	escape("unable to erase the block from the hole");

      leave();
    }

  }
}
