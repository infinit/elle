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
// updated       julien quintard   [thu jan  7 13:18:40 2010]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the expiration delays for blocks depending on their family.
    ///
    core::Time		Depot::Delays[hole::Families];

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the depot.
    ///
    Status		Depot::Initialize()
    {
      // set the content hash block.
      Depot::Delays[hole::FamilyContentHashBlock].year =
	Variable::Maximum(Depot::Delays[hole::FamilyContentHashBlock].year);

      // set the public key blocks delay.
      if (Depot::Delays[hole::FamilyPublicKeyBlock].Current() ==
	  StatusError)
	escape("unable to get the current time");

      Depot::Delays[hole::FamilyPublicKeyBlock].minute = 5;

      leave();
    }

    ///
    /// this method cleans the depot.
    ///
    Status		Depot::Clean()
    {
      // XXX

      leave();
    }

    ///
    /// this method stores a block by updating the cache.
    ///
    Status		Depot::Put(const hole::Address&		address,
				   hole::Block*			block)
    {
      /* XXX
      // update the cache.
      if (Cache::Put(address,
		     block,
		     Depot::Delays[block->family]) == StatusError)
	escape("unable to put the block in the cache");
      */

      leave();
    }

    ///
    /// this method retrieves a block from the storage layer.
    ///
    Status		Depot::Get(const hole::Address&		address,
				   hole::Block*&		block)
    {
      /* XXX
      // look into the journal.
      // XXX

      // look in the cache.
      if (Cache::Get(address, block) == StatusOk)
	leave();

      // look in the house.
      // XXX

      // finally, look in the hole.
      if (hole::Hole::Get(address, block) == StatusOk)
	leave();

      escape("unable to find the block");
      */
    }

  }
}
