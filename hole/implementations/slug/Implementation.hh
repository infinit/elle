//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 31 14:50:02 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_SLUG_IMPLEMENTATION_HH
#define HOLE_IMPLEMENTATIONS_SLUG_IMPLEMENTATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>

#include <hole/Holeable.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// the slug hole implementation.
      ///
      class Implementation:
        public Holeable
      {
      public:
        //
        // constructors & destructors
        //
        Implementation(const nucleus::Network&);

        //
        // interfaces
        //

        // holeable
        void                    Join();
        elle::Status            Leave();

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

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;
      };

    }
  }
}

#endif
