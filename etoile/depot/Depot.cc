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
// updated       julien quintard   [tue sep  1 01:53:48 2009]
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
    /// XXX
    ///
    Status		Depot::Get(const hole::Address&		address,
				   hole::Block&			block)
    {
      // look into the journal.
      // XXX

      // look in the cache.
      // XXX

      // look in the store.
      // XXX

      // finally, look in the hole.
      if (hole::Hole::Get(address, block) == StatusOk)
	leave();

      escape("unable to find the block");
    }

    ///
    /// XXX
    ///
    Status		Depot::Put(const hole::Address&		address,
				   const hole::Block&		block)
    {
      // update the cache.
      // XXX

      // update the hole.
      if (hole::Hole::Put(address, block) == StatusError)
	escape("unable to put the block in the hole");

      leave();
    }

  }
}
