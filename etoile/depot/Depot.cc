#include <etoile/depot/Depot.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Revision.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Ensemble.hh>

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
      address = hole::Hole::instance().origin();
      return elle::Status::Ok;
    }

    ///
    /// this method stores the given block in the underlying storage layer.
    ///
    elle::Status        Depot::Push(const nucleus::proton::Address& address,
                                    const nucleus::proton::Block& block)
    {
      hole::Hole::instance().push(address, block);
      return elle::Status::Ok;
    }

    std::unique_ptr<nucleus::neutron::Object>
    Depot::pull_object(nucleus::proton::Address const& address,
                       nucleus::proton::Revision const & revision)
    {
      return (Depot::pull<nucleus::neutron::Object>(address, revision));
    }

    std::unique_ptr<nucleus::neutron::Access>
    Depot::pull_access(nucleus::proton::Address const& address)
    {
      return (Depot::pull<nucleus::neutron::Access>(address));
    }

    std::unique_ptr<nucleus::neutron::Group>
    Depot::pull_group(nucleus::proton::Address const& address,
                      nucleus::proton::Revision const& revision)
    {
      return (Depot::pull<nucleus::neutron::Group>(address, revision));
    }

    std::unique_ptr<nucleus::neutron::Ensemble>
    Depot::pull_ensemble(nucleus::proton::Address const& address)
    {
      return (Depot::pull<nucleus::neutron::Ensemble>(address));
    }

    ///
    /// this method permanently removes a block from the storage layer.
    ///
    elle::Status        Depot::Wipe(const nucleus::proton::Address& address)
    {
      // call the Hole.
      hole::Hole::instance().wipe(address);

      return elle::Status::Ok;
    }

  }
}
