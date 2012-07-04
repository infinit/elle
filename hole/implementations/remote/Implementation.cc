#include <hole/implementations/remote/Implementation.hh>
#include <hole/implementations/remote/Remote.hh>
#include <hole/implementations/remote/Machine.hh>
#include <hole/implementations/remote/Client.hh>

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

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Implementation::Put(
                          const nucleus::proton::Address& address,
                          const nucleus::proton::ImmutableBlock& block)
      {
        // check if the machine is a client.
        if (Remote::Computer->role != Machine::RoleClient)
          escape("the hole is not acting as a remote client as it should");

        // forward the request to the machine.
        if (Remote::Computer->client->Put(address, block) == elle::Status::Error)
          escape("unable to put the block");

        return elle::Status::Ok;
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status      Implementation::Put(
                          const nucleus::proton::Address& address,
                          const nucleus::proton::MutableBlock& block)
      {
        // check if the machine is a client.
        if (Remote::Computer->role != Machine::RoleClient)
          escape("the hole is not acting as a remote client as it should");

        // forward the request to the machine.
        if (Remote::Computer->client->Put(address, block) == elle::Status::Error)
          escape("unable to put the block");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status      Implementation::Get(
                          const nucleus::proton::Address& address,
                          nucleus::proton::ImmutableBlock& block)
      {
        // check if the machine is a client.
        if (Remote::Computer->role != Machine::RoleClient)
          escape("the hole is not acting as a remote client as it should");

        // forward the request to the machine.
        if (Remote::Computer->client->Get(address, block) == elle::Status::Error)
          escape("unable to get the block");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status      Implementation::Get(
                          const nucleus::proton::Address& address,
                          const nucleus::proton::Version& version,
                          nucleus::proton::MutableBlock& block)
      {
        // check if the machine is a client.
        if (Remote::Computer->role != Machine::RoleClient)
          escape("the hole is not acting as a remote client as it should");

        // forward the request to the machine.
        if (Remote::Computer->client->Get(address,
                                          version,
                                          block) == elle::Status::Error)
          escape("unable to get the block");

        return elle::Status::Ok;
      }

      ///
      /// this method removes a block.
      ///
      elle::Status      Implementation::Kill(
                          const nucleus::proton::Address& address)
      {
        // check if the machine is a client.
        if (Remote::Computer->role != Machine::RoleClient)
          escape("the hole is not acting as a remote client as it should");

        // forward the request to the machine.
        if (Remote::Computer->client->Kill(address) == elle::Status::Error)
          escape("unable to kill the block");

        return elle::Status::Ok;
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the implementation.
      ///
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
