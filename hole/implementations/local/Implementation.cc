
#include <elle/utility/Time.hh>
#include <elle/concurrency/Timer.hh>

#include <hole/implementations/local/Implementation.hh>
#include <hole/implementations/local/Local.hh>
#include <hole/Hole.hh>

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
      Implementation::Implementation(const nucleus::Network&    network):
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
      elle::Status      Implementation::Join()
      {
        // allocate the machine.
        Local::Computer = new Machine;

        // set the hole as ready to receive requests.
        if (Hole::Ready() == elle::Status::Error)
          escape("unable to set the hole online");

        return elle::Status::Ok;
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

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Implementation::Put(
                          const nucleus::Address&               address,
                          const nucleus::ImmutableBlock&         block)
      {
        // forward the request to the machine.
        if (Local::Computer->Put(address, block) == elle::Status::Error)
          escape("unable to put the block");

        return elle::Status::Ok;
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status      Implementation::Put(
                          const nucleus::Address&               address,
                          const nucleus::MutableBlock&          block)
      {
        // forward the request to the machine.
        if (Local::Computer->Put(address, block) == elle::Status::Error)
          escape("unable to put the block");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status      Implementation::Get(
                          const nucleus::Address&               address,
                          nucleus::ImmutableBlock&              block)
      {
        // forward the request to the machine.
        if (Local::Computer->Get(address, block) == elle::Status::Error)
          escape("unable to get the block");

        return elle::Status::Ok;
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status      Implementation::Get(
                          const nucleus::Address&               address,
                          const nucleus::Version&               version,
                          nucleus::MutableBlock&                block)
      {
        // forward the request to the machine.
        if (Local::Computer->Get(address,
                                 version,
                                 block) == elle::Status::Error)
          escape("unable to get the block");

        return elle::Status::Ok;
      }

      ///
      /// this method removes a block.
      ///
      elle::Status      Implementation::Kill(
                          const nucleus::Address&               address)
      {
        // forward the request to the machine.
        if (Local::Computer->Kill(address) == elle::Status::Error)
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
