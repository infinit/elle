#include <hole/implementations/local/Implementation.hh>
#include <hole/implementations/local/Local.hh>
#include <hole/implementations/local/Machine.hh>
#include <hole/Hole.hh>

#include <nucleus/proton/fwd.hh>
#include <nucleus/proton/Block.hh>

#include <elle/idiom/Open.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.hole.implementations.local.Implementation");

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
        ELLE_DEBUG_METHOD("");

        Local::Computer = new Machine(*this);
        this->ready();
      }

      void
      Implementation::_leave()
      {
        ELLE_DEBUG_METHOD("");

        delete Local::Computer;
      }

      /*---------------.
      | Implementation |
      `---------------*/

      void
      Implementation::_push(nucleus::proton::Address const& address,
                            nucleus::proton::ImmutableBlock const& block)
      {
        ELLE_DEBUG_METHOD(address, block);

        Local::Computer->put(address, block);
      }

      void
      Implementation::_push(nucleus::proton::Address const& address,
                            nucleus::proton::MutableBlock const& block)
      {
        ELLE_DEBUG_METHOD(address, block);

        Local::Computer->put(address, block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(nucleus::proton::Address const& address)
      {
        ELLE_DEBUG_METHOD(address);

        return Local::Computer->get(address);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(nucleus::proton::Address const& address,
                            nucleus::proton::Revision const& revision)
      {
        ELLE_DEBUG_METHOD(address, revision);

        return Local::Computer->get(address, revision);
      }

      void
      Implementation::_wipe(nucleus::proton::Address const& address)
      {
        ELLE_DEBUG_METHOD(address);

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
