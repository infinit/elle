#include <hole/implementations/slug/Implementation.hh>
#include <hole/implementations/slug/Slug.hh>
#include <hole/implementations/slug/Machine.hh>

#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Network.hh>

#include <elle/idiom/Open.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor.
      ///
      Implementation::Implementation(
        hole::storage::Storage& storage,
        elle::Passport const& passport,
        elle::Authority const& authority,
        std::vector<elle::network::Locus> const& members,
        int port,
        reactor::Duration connection_timeout):
        Hole(storage, passport, authority),
        _members(members),
        _port(port),
        _connection_timeout(connection_timeout)
      {}

      /*------------.
      | Join, leave |
      `------------*/

      void
      Implementation::_join()
      {
        Slug::Computer = new Machine(*this, this->_port,
                                     this->_connection_timeout);
      }

      void
      Implementation::_leave()
      {
        delete Slug::Computer;
      }

      void
      Implementation::_push(const nucleus::proton::Address& address,
                            const nucleus::proton::ImmutableBlock& block)
      {
        Slug::Computer->put(address, block);
      }

      void
      Implementation::_push(const nucleus::proton::Address& address,
                            const nucleus::proton::MutableBlock& block)
      {
        Slug::Computer->put(address, block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(const nucleus::proton::Address& address)
      {
        return Slug::Computer->get(address);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(const nucleus::proton::Address& address,
                            const nucleus::proton::Revision& revision)
      {
        return Slug::Computer->get(address, revision);
      }

      void
      Implementation::_wipe(const nucleus::proton::Address& address)
      {
        Slug::Computer->wipe(address);
      }

      /*---------.
      | Dumpable |
      `---------*/

      elle::Status
      Implementation::Dump(const elle::Natural32 margin) const
      {
        elle::String    alignment(margin, ' ');

        std::cout << alignment << "[Implementation] Slug" << std::endl;

        // dump the machine.
        if (Slug::Computer->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the machine");

        return elle::Status::Ok;
      }

    }
  }
}
