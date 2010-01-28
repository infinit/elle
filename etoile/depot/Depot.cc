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
// updated       julien quintard   [wed jan 27 21:32:55 2010]
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
    Status		Depot::Initialize()
    {
      return (Repository::Initialize());
    }

    ///
    /// this method cleans the depot.
    ///
    Status		Depot::Clean()
    {
      return (Repository::Clean());
    }

    ///
    /// this method stores a block by updating the cache.
    ///
    Status		Depot::Put(const hole::Address&		address,
				   hole::Block*			block,
				   const Natural32		size)
    {
      // put in the repository.
      if (Repository::Put(address, block, size) == StatusError)
	escape("unable to put the block in the repository");

      leave();
    }

    ///
    /// this method retrieves a block from the storage layer.
    ///
    Status		Depot::Get(const hole::Address&		address,
				   hole::Block*&		block)
    {
      // look in the journal.
      // XXX

      // look in the repository.
      if (Repository::Get(address, block) == StatusOk)
	leave();

      // finally, look in the hole.
      if (hole::Hole::Get(address, block) == StatusOk)
	leave();

      escape("unable to find the block");
    }

  }
}
