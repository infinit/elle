#include <hole/implementations/local/Implementation.hh>
#include <hole/implementations/local/Local.hh>
#include <hole/implementations/local/Machine.hh>
#include <hole/Hole.hh>

#include <nucleus/proton/fwd.hh>

#include <elle/idiom/Open.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
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
        // allocate the machine.
        Local::Computer = new Machine;

        Hole::ready();
      }

      ///
      /// this method cleans the peer.
      ///
      elle::Status      Implementation::Leave()
      {
        // delete the machine.
        delete Local::Computer;

        return elle::Status::Ok;
      }

      void
      Implementation::Put(const nucleus::proton::Address& address,
                          const nucleus::proton::ImmutableBlock& block)
      {
        Local::Computer->Put(address, block);
      }

      void
      Implementation::Put(const nucleus::proton::Address&               address,
                          const nucleus::proton::MutableBlock&          block)
      {
        Local::Computer->Put(address, block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::Get(const nucleus::proton::Address& address)
      {
        return Local::Computer->Get(address);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::Get(const nucleus::proton::Address& address,
                          const nucleus::proton::Revision& revision)
      {
        return Local::Computer->Get(address, revision);
      }

      void
      Implementation::Kill(const nucleus::proton::Address& address)
      {
        Local::Computer->Kill(address);
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

        std::cout << alignment << "[Implementation] Local" << std::endl;

        // dump the parent.
        if (Holeable::Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the holeabl");

        // dump the machine.
        if (Local::Computer->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the machine");

        return elle::Status::Ok;
      }

    }
  }
}
