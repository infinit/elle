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
// updated       julien quintard   [mon may  3 18:46:18 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Depot.hh>

#include <etoile/hole/Hole.hh>

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
      return (Repository::Initialize());
    }

    ///
    /// this method cleans the depot.
    ///
    elle::Status	Depot::Clean()
    {
      return (Repository::Clean());
    }

    ///
    /// this method stores a block by updating the repository.
    ///
    elle::Status	Depot::Put(const hole::Address&		address,
				   hole::Block*			block)
    {
      enter();

      // update in the repository.
      //if (Repository::Put(address, block) == elle::StatusError)
      //escape("unable to put the block in the repository");

      leave();
    }

    ///
    /// this method retrieves a block from the storage layer.
    ///
    elle::Status	Depot::Get(const hole::Address&		address,
				   hole::Block*&		block)
    {
      enter();

      // look in the journal.
      // XXX

      // look in the repository.
      //if (Repository::Get(address, block) == elle::StatusTrue)
      //leave();

      // finally, look in the hole.
      if (hole::Hole::Get(address, block) == elle::StatusTrue)
	leave();

      escape("unable to find the block");
    }

  }
}
