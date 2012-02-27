//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 31 13:52:28 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_LOCAL_MACHINE_HH
#define HOLE_IMPLEMENTATIONS_LOCAL_MACHINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// this class represents the current host.
      ///
      /// note that every host can emulate multiple nodes though in the
      /// case of the local implementation, a single host is present i.e
      /// the machine.
      ///
      class Machine:
        public elle::Entity
      {
      public:
        //
        // methods
        //
        elle::Status            Put(const nucleus::Address&,
                                    const nucleus::ImmutableBlock&);
        elle::Status            Put(const nucleus::Address&,
                                    const nucleus::MutableBlock&);
        elle::Status            Get(const nucleus::Address&,
                                    nucleus::ImmutableBlock&);
        elle::Status            Get(const nucleus::Address&,
                                    const nucleus::Version&,
                                    nucleus::MutableBlock&);
        elle::Status            Kill(const nucleus::Address&);

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;
      };

    }
  }
}

#endif
