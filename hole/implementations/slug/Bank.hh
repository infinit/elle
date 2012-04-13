//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [mon dec  5 14:41:58 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_SLUG_BANK_HH
#define HOLE_IMPLEMENTATIONS_SLUG_BANK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

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
      class Bank:
        public elle::Object
      {
      public:
        //
        // methods
        //
        // XXX

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        // XXX
      };

    }
  }
}

#endif
