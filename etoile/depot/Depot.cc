#include <etoile/depot/Depot.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Version.hh>

#include <hole/Hole.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- methods ---------------------------------------------------------
//

    /// this method returns the address of the network's root block.
    elle::Status        Depot::Origin(nucleus::proton::Address& address)
    {
      // call the Hole.
      if (hole::Hole::Origin(address) == elle::Status::Error)
        escape("unable to retrieve the origin");

      return elle::Status::Ok;
    }

    ///
    /// this method stores the given block in the underlying storage layer.
    ///
    elle::Status        Depot::Push(const nucleus::proton::Address& address,
                                    const nucleus::proton::Block& block)
    {
      // call the Hole.
      if (hole::Hole::Push(address, block) == elle::Status::Error)
        escape("unable to store the block");

      return elle::Status::Ok;
    }

    ///
    /// this method retrives a block from the underlying storage layer.
    ///
    elle::Status        Depot::Pull(const nucleus::proton::Address& address,
                                    const nucleus::proton::Version& version,
                                    nucleus::proton::Block& block)
    {
      // call the Hole.
      if (hole::Hole::Pull(address, version, block) == elle::Status::Error)
        escape("unable to retrieve the block");

      return elle::Status::Ok;
    }

    ///
    /// this method permanently removes a block from the storage layer.
    ///
    elle::Status        Depot::Wipe(const nucleus::proton::Address& address)
    {
      // call the Hole.
      if (hole::Hole::Wipe(address) == elle::Status::Error)
        escape("unable to remove the block");

      return elle::Status::Ok;
    }

  }
}
