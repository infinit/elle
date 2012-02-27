//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 24 10:33:01 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOUR_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOUR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <hole/Label.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Neighbour:
        public elle::Entity
      {
      public:
        //
        // constants
        //
        static const elle::Natural32            Timeout;

        //
        // enumerations
        //
        enum State
          {
            StateUnauthenticated,
            StateAuthenticated
          };

        //
        // constructors & destructors
        //
        Neighbour();
        ~Neighbour();

        //
        // methods
        //
        elle::Status    Create(const elle::Locus&);
        elle::Status    Create(elle::Gate*);

        elle::Status    Connect();

        //
        // callbacks
        //
        elle::Status    Discard();

        elle::Status    Monitor();

        //
        // interfaces
        //

        // dumpable
        elle::Status    Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        State           state;

        elle::Locus     locus;
        Label           label;

        elle::Port      port;

        elle::Gate*     gate;

        elle::Timer*    timer;
      };

    }
  }
}

#endif
