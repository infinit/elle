#include <etoile/depot/Depot.hh>

#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>
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

    std::unique_ptr<nucleus::neutron::Object>
    Depot::pull_object(nucleus::proton::Address const& address,
                       nucleus::proton::Version const & version)
    {
      std::unique_ptr<nucleus::proton::Block> block;

      block = hole::Hole::Pull(address, version);

      std::unique_ptr<nucleus::neutron::Object> object(
        dynamic_cast<nucleus::neutron::Object*>(block.release()));

      if (!object)
        throw std::runtime_error("the retrieved block is not an object");

      return object;
    }

    std::unique_ptr<nucleus::neutron::Access>
    Depot::pull_access(nucleus::proton::Address const& address)
    {
      std::unique_ptr<nucleus::proton::Block> block;

      block = hole::Hole::Pull(address, nucleus::proton::Version::Last);

      std::unique_ptr<nucleus::neutron::Access> access(
        dynamic_cast<nucleus::neutron::Access*>(block.release()));

      if (!access)
        throw std::runtime_error("the retrieved block is not an access");

      return access;
    }

    ///
    /// this method permanently removes a block from the storage layer.
    ///
    elle::Status        Depot::Wipe(const nucleus::proton::Address& address)
    {
      // call the Hole.
      hole::Hole::Wipe(address);

      return elle::Status::Ok;
    }

  }
}
