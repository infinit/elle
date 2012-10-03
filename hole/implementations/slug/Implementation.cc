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
        std::vector<elle::network::Locus> const& members,
        int port):
        Hole(storage),
        _members(members),
        _port(port)
      {}

      /*------------.
      | Join, leave |
      `------------*/

      void
      Implementation::_join()
      {
        Slug::Computer = new Machine(*this, this->_port);
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
        Slug::Computer->Put(address, block);
      }

      void
      Implementation::_push(const nucleus::proton::Address& address,
                            const nucleus::proton::MutableBlock& block)
      {
        Slug::Computer->Put(address, block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(const nucleus::proton::Address& address)
      {
        return Slug::Computer->Get(address);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::_pull(const nucleus::proton::Address& address,
                          const nucleus::proton::Revision& revision)
      {
        return Slug::Computer->Get(address, revision);
      }

      void
      Implementation::_wipe(const nucleus::proton::Address& address)
      {
        Slug::Computer->Kill(address);
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
