//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue sep  1 01:11:07 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Depot.hh>

#include <hole/Hole.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- methods ---------------------------------------------------------
//

    /// this method returns the address of the network's root block.
    elle::Status        Depot::Origin(nucleus::Address&         address)
    {
      // call the Hole.
      if (hole::Hole::Origin(address) == elle::Status::Error)
        escape("unable to retrieve the origin");

      return elle::Status::Ok;
    }

    ///
    /// this method stores the given block in the underlying storage layer.
    ///
    elle::Status        Depot::Push(const nucleus::Address&     address,
                                    const nucleus::Block&       block)
    {
      // call the Hole.
      if (hole::Hole::Push(address, block) == elle::Status::Error)
        escape("unable to store the block");

      return elle::Status::Ok;
    }

    ///
    /// this method retrives a block from the underlying storage layer.
    ///
    elle::Status        Depot::Pull(const nucleus::Address&     address,
                                    const nucleus::Version&     version,
                                    nucleus::Block&             block)
    {
      // call the Hole.
      if (hole::Hole::Pull(address, version, block) == elle::Status::Error)
        escape("unable to retrieve the block");

      return elle::Status::Ok;
    }

    ///
    /// this method permanently removes a block from the storage layer.
    ///
    elle::Status        Depot::Wipe(const nucleus::Address&     address)
    {
      // call the Hole.
      if (hole::Hole::Wipe(address) == elle::Status::Error)
        escape("unable to remove the block");

      return elle::Status::Ok;
    }

  }
}
