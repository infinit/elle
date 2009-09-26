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
// updated       julien quintard   [sat sep 12 00:36:12 2009]
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
    /// this method computes immutable content hash blocks' expiration time.
    ///
    Status		Depot::Expiration(const core::ContentHashBlock&,
					  core::Time&		expiration)
    {
      // build an infinit expiration date.
      expiration.year = Variable::Maximum(expiration.year);

      leave();
    }

    ///
    /// this method compute public key blocks' expiration time.
    ///
    Status		Depot::Expiration(const core::PublicKeyBlock&,
					  core::Time&		expiration)
    {
      // get the current date.
      if (expiration.Current() == StatusError)
	escape("unable to get the current time");

      // add the delay during which a block can survive in the depot.
      expiration = expiration + Depot::Delays::PublicKeyBlock;

      leave();
    }

  }
}
