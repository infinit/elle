#include <hole/implementations/remote/Implementation.hh>
#include <hole/implementations/remote/Remote.hh>
#include <hole/implementations/remote/Machine.hh>
#include <hole/implementations/remote/Client.hh>

#include <elle/concurrency/Scheduler.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Open.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor.
      ///
      Implementation::Implementation(const nucleus::proton::Network& network):
        Holeable(network)
      {
      }

//
// ---------- holeable --------------------------------------------------------
//

      ///
      /// this method tries to connect to the server. if impossible, a server
      /// is author .
      ///
      void
      Implementation::Join()
      {
        Remote::Computer = new Machine;
        Remote::Computer->Launch();
      }

      ///
      /// this method cleans the peer.
      ///
      elle::Status      Implementation::Leave()
      {
        // delete the machine.
        delete Remote::Computer;

        return elle::Status::Ok;
      }

      void
      Implementation::Put(const nucleus::proton::Address& address,
                          const nucleus::proton::ImmutableBlock& block)
      {
        if (Remote::Computer->role != Machine::RoleClient)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the hole is not acting as a remote client as it should");
        Remote::Computer->client->put(address, block);
      }

      void
      Implementation::Put(const nucleus::proton::Address& address,
                          const nucleus::proton::MutableBlock& block)
      {
        if (Remote::Computer->role != Machine::RoleClient)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the hole is not acting as a remote client as it should");
        Remote::Computer->client->put(address, block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::Get(const nucleus::proton::Address& address)
      {
        if (Remote::Computer->role != Machine::RoleClient)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the hole is not acting as a remote client as it should");
        return Remote::Computer->client->get(address);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::Get(const nucleus::proton::Address& address,
                          const nucleus::proton::Revision& revision)
      {
        if (Remote::Computer->role != Machine::RoleClient)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the hole is not acting as a remote client as it should");
        return Remote::Computer->client->get(address, revision);
      }

      void
      Implementation::Kill(const nucleus::proton::Address& address)
      {
        if (Remote::Computer->role != Machine::RoleClient)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the hole is not acting as a remote client as it should");
        Remote::Computer->client->kill(address);
      }

      /*---------.
      | Dumpable |
      `---------*/

      elle::Status      Implementation::Dump(const elle::Natural32 margin)
        const
      {
        elle::String    alignment(margin, ' ');

        std::cout << alignment << "[Implementation] Remote" << std::endl;

        // dump the parent.
        if (Holeable::Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the holeabl");

        // dump the machine.
        if (Remote::Computer->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the machine");

        return elle::Status::Ok;
      }

    }
  }
}
