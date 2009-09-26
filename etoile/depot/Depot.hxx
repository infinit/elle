//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Depot.hxx
//
// created       julien quintard   [sat sep 12 00:00:46 2009]
// updated       julien quintard   [sat sep 12 00:08:39 2009]
//

#ifndef ETOILE_DEPOT_DEPOT_HXX
#define ETOILE_DEPOT_DEPOT_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Cache.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method retrieves a block starting looking in the journal, then
    /// the cache, then the house and finally the hole.
    ///
    template <typename T>
    Status		Depot::Get(const hole::Address&		address,
				   T&				block)
    {
      // look into the journal.
      // XXX

      // look in the cache.
      if (depot::Cache::Get(address, block) == StatusOk)
	leave();

      // look in the house.
      // XXX

      // finally, look in the hole.
      if (hole::Hole::Get(address, block) == StatusOk)
	leave();

      escape("unable to find the block");
    }

    ///
    /// this method stores a block by updating the cache then the hole.
    ///
    template <typename T>
    Status		Depot::Put(const hole::Address&		address,
				   const T&			block)
    {
      core::Time	expiration;

      // compute the block's expiration time, depending on its type.
      if (depot::Depot::Expiration(block, expiration) == StatusError)
	escape("unable to compute the expiration time");

      // update the cache.
      if (depot::Cache::Put(address, block, expiration) == StatusError)
	escape("unable to put the block in the cache");

      // update the hole.
      // XXX just for the test.
      //if (hole::Hole::Put(address, block) == StatusError)
      //escape("unable to put the block in the hole");

      leave();
    }

  }
}

#endif
