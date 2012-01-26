//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [mon dec  5 14:21:56 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_SLUG_SYNCHRONISER_HH
#define HOLE_IMPLEMENTATIONS_SLUG_SYNCHRONISER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
      /// XXX
      ///
      class Synchroniser:
        public elle::Entity
      {
      public:
        //
        // methods
        //
        elle::Status            Start();

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // callbacks
        //
        elle::Status            Run();

        //
        // signals
        //
        struct
        {
          elle::Signal<
            elle::Parameters<
              >
            >                   synchronised;
        }                       signal;

        //
        // attributes
        //
        elle::Timer             timer;
      };

    }
  }
}

#endif
