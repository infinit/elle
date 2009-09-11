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
// updated       julien quintard   [fri sep 11 01:37:14 2009]
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
    /// the expiration delay for blocks to survive in the depot without
    /// being refreshed from the hole.
    ///
    core::Time		Depot::Delays::PublicKeyBlock;

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
	// set the public key blocks delay.
	Depot::Delays::PublicKeyBlock.minute = 5;
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
    /// this method retrives a block starting looking in the journal, then
    /// the cache, then the house and finally the hole.
    ///
    Status		Depot::Get(const hole::Address&		address,
				   hole::Block&			block)
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
    Status		Depot::Put(const hole::Address&		address,
				   const hole::Block&		block,
				   const core::Time&		expiration)
    {
      // update the cache.
      if (depot::Cache::Put(address, block, expiration) == StatusError)
	escape("unable to put the block in the cache");

      // update the hole.
      // XXX just for the test.
      //if (hole::Hole::Put(address, block) == StatusError)
      //escape("unable to put the block in the hole");
    }

    ///
    /// this method stores a immutable content hash block.
    ///
    /// note that such blocks cannot expire.
    ///
    Status		Depot::Put(const hole::Address&		address,
				   const core::ContentHashBlock&block)
    {
      core::Time	expiration;

      // build an infinit expiration date.
      {
	// set a maximum year.
	expiration.year = Variable::Maximum(expiration.year);
      }

      // store the block
      if (Depot::Put(address, block, expiration) == StatusError)
	escape("unable to store the block");

      leave();
    }

    ///
    /// this method stores a public key block.
    ///
    Status		Depot::Put(const hole::Address&		address,
				   const core::PublicKeyBlock&	block)
    {
      core::Time	expiration;

      // build the expiration date.
      {
	// get the current date.
	if (expiration.Current() == StatusError)
	  escape("unable to get the current time");

	// add the delay during which a block can survive in the depot.
	expiration = expiration + Depot::Delays::PublicKeyBlock;
      }

      // store the block
      if (Depot::Put(address, block, expiration) == StatusError)
	escape("unable to store the block");

      leave();
    }

  }
}
