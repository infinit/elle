#include <hole/implementations/local/Implementation.hh>
#include <hole/implementations/local/Local.hh>
#include <hole/implementations/local/Machine.hh>
#include <hole/Hole.hh>

#include <nucleus/proton/fwd.hh>
#include <nucleus/proton/Block.hh>

#include <elle/idiom/Open.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {
      /*-------------.
      | Construction |
      `-------------*/

      Implementation::Implementation(storage::Storage& storage):
        Hole(storage)
      {}

      /*------------.
      | Join, leave |
      `------------*/

      void
      Implementation::_join()
      {
        Local::Computer = new Machine(*this);
        this->ready();
      }

      void
      Implementation::_leave()
      {
        delete Local::Computer;
      }

      /*---------------.
      | Implementation |
      `---------------*/

      void
      Implementation::_push(nucleus::proton::Address const& address,
                            nucleus::proton::ImmutableBlock const& block)
      {
        Local::Computer->Put(address, block);
      }

      void
      Implementation::_push(nucleus::proton::Address const& address,
                            nucleus::proton::MutableBlock const& block)
      {
        Local::Computer->Put(address, block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(nucleus::proton::Address const& address)
      {
        return Local::Computer->Get(address);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(nucleus::proton::Address const& address,
                            nucleus::proton::Revision const& revision)
      {
        return Local::Computer->Get(address, revision);
      }

      void
      Implementation::_wipe(nucleus::proton::Address const& address)
      {
        Local::Computer->Kill(address);
      }

      /*---------.
      | Dumpable |
      `---------*/

      elle::Status
      Implementation::Dump(const elle::Natural32 margin) const
      {
        elle::String    alignment(margin, ' ');

        std::cout << alignment << "[Implementation] Local" << std::endl;

        // dump the machine.
        if (Local::Computer->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the machine");

        return elle::Status::Ok;
      }

    }
  }
}
