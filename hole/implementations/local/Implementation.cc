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

      Implementation::Implementation(storage::Storage& storage,
                                     elle::Passport const& passport,
                                     elle::Authority const& authority):
        Hole(storage, passport, authority)
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
        Local::Computer->put(address, block);
      }

      void
      Implementation::_push(nucleus::proton::Address const& address,
                            nucleus::proton::MutableBlock const& block)
      {
        Local::Computer->put(address, block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(nucleus::proton::Address const& address)
      {
        return Local::Computer->get(address);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(nucleus::proton::Address const& address,
                            nucleus::proton::Revision const& revision)
      {
        return Local::Computer->get(address, revision);
      }

      void
      Implementation::_wipe(nucleus::proton::Address const& address)
      {
        Local::Computer->wipe(address);
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
