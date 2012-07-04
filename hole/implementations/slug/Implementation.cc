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
        Slug::Computer = new Machine;

        // launch it.
        if (Slug::Computer->Launch() == elle::Status::Error)
          throw std::runtime_error("unable to launch the computer");
      }

      ///
      /// this method cleans the peer.
      ///
      elle::Status      Implementation::Leave()
      {
        // delete the machine.
        delete Slug::Computer;

        return elle::Status::Ok;
      }

      void
      Implementation::Put(const nucleus::proton::Address& address,
                          const nucleus::proton::ImmutableBlock& block)
      {
        Slug::Computer->Put(address, block);
      }

      void
      Implementation::Put(const nucleus::proton::Address& address,
                          const nucleus::proton::MutableBlock& block)
      {
        Slug::Computer->Put(address, block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::Get(const nucleus::proton::Address& address)
      {
        return Slug::Computer->Get(address);
      }

      std::unique_ptr<nucleus::proton::Block>
      Implementation::Get(const nucleus::proton::Address& address,
                          const nucleus::proton::Version& version)
      {
        return Slug::Computer->Get(address, version);
      }

      void
      Implementation::Kill(const nucleus::proton::Address& address)
      {
        Slug::Computer->Kill(address);
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

        std::cout << alignment << "[Implementation] Slug" << std::endl;

        // dump the parent.
        if (Holeable::Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the holeabl");

        // dump the machine.
        if (Slug::Computer->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the machine");

        return elle::Status::Ok;
      }

    }
  }
}
