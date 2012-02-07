//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 31 14:49:56 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/cirkle/Implementation.hh>
#include <hole/implementations/cirkle/Cirkle.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
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
        ;

        // allocate the machine.
        Cirkle::Computer = new Machine;

        // launch it.
        if (Cirkle::Computer->Launch() == elle::StatusError)
          escape("unable to launch the computer");

        return elle::StatusOk;
      }

      ///
      /// this method cleans the peer.
      ///
      elle::Status      Implementation::Leave()
      {
        ;

        // delete the machine.
        delete Cirkle::Computer;

        return elle::StatusOk;
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status      Implementation::Put(
                          const nucleus::Address&               address,
                          const nucleus::ImmutableBlock&         block)
      {
        ;

        // forward the request to the machine.
        if (Cirkle::Computer->Put(address, block) == elle::StatusError)
          escape("unable to put the block");

        return elle::StatusOk;
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status      Implementation::Put(
                          const nucleus::Address&               address,
                          const nucleus::MutableBlock&          block)
      {
        ;

        // forward the request to the machine.
        if (Cirkle::Computer->Put(address, block) == elle::StatusError)
          escape("unable to put the block");

        return elle::StatusOk;
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status      Implementation::Get(
                          const nucleus::Address&               address,
                          nucleus::ImmutableBlock&              block)
      {
        ;

        // forward the request to the machine.
        if (Cirkle::Computer->Get(address, block) == elle::StatusError)
          escape("unable to get the block");

        return elle::StatusOk;
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status      Implementation::Get(
                          const nucleus::Address&               address,
                          const nucleus::Version&               version,
                          nucleus::MutableBlock&                block)
      {
        ;

        // forward the request to the machine.
        if (Cirkle::Computer->Get(address,
                                  version,
                                  block) == elle::StatusError)
          escape("unable to get the block");

        return elle::StatusOk;
      }

      ///
      /// this method removes a block.
      ///
      elle::Status      Implementation::Kill(
                          const nucleus::Address&               address)
      {
        ;

        // forward the request to the machine.
        if (Cirkle::Computer->Kill(address) == elle::StatusError)
          escape("unable to kill the block");

        return elle::StatusOk;
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

        ;

        std::cout << alignment << "[Implementation] Cirkle" << std::endl;

        // dump the parent.
        if (Holeable::Dump(margin + 2) == elle::StatusError)
          escape("unable to dump the holeabl");

        // dump the machine.
        if (Cirkle::Computer->Dump(margin + 2) == elle::StatusError)
          escape("unable to dump the machine");

        return elle::StatusOk;
      }

    }
  }
}
