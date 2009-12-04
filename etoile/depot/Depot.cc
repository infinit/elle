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
// updated       julien quintard   [fri dec  4 12:20:57 2009]
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
    core::Time		Depot::Delays[hole::Block::Families];

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the depot.
    ///
    Status		Depot::Initialize()
    {
      // initialize the delays.
      {
	// set the content hash block.
	Depot::Delays[hole::Block::FamilyContentHashBlock].year =
	  Variable::Maximum(Depot::Delays[hole::Block::FamilyContentHashBlock].year);

	// set the public key blocks delay.
	if (Depot::Delays[hole::Block::FamilyPublicKeyBlock].Current() ==
	    StatusError)
	  escape("unable to get the current time");

	Depot::Delays[hole::Block::FamilyPublicKeyBlock].minute = 5;
      }

      // initialize the cache.
      if (Cache::Initialize() == StatusError)
	escape("unable to initialize the cache");

      // initialize the house.
      // XXX

      leave();
    }

    ///
    /// this method cleans the depot.
    ///
    Status		Depot::Clean()
    {
      // clean the cache.
      if (Cache::Clean() == StatusError)
	escape("unable to clean the cache");

      // clean the house.
      // XXX

      leave();
    }

    ///
    /// this method stores a block by updating the cache.
    ///
    Status		Depot::Put(const hole::Address&		address,
				   hole::Block*			block)
    {
      // update the cache.
      if (Cache::Put(address,
		     block,
		     Depot::Delays[block->family]) == StatusError)
	escape("unable to put the block in the cache");

      leave();
    }

    ///
    /// this method retrieves a block from the storage layer.
    ///
    Status		Depot::Get(const hole::Address&		address,
				   hole::Block*&		block)
    {
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
    }

  }
}
