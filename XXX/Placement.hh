//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [sun dec 25 14:07:49 2011]
//

#ifndef NUCLEUS_PROTON_PLACEMENT_HH
#define NUCLEUS_PROTON_PLACEMENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// XXX
    ///
    class Placement:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Placement    Null;

      //
      // constructors & destructors
      //
      Placement();

      //
      // interfaces
      //

      // object
      declare(Placement);
      elle::Boolean             operator==(const Placement&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::Natural64           number;
    };

  }
}

#endif
