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

#include <elle/types.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// XXX
    ///
    class Placement:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const elle::Natural64      Zero;

      static const Placement            Null;

      //
      // static attribute
      //
      static elle::Natural64            Counter;

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
      elle::Boolean             operator<(const Placement&) const;

      // archivable
      //elle::Status              Serialize(elle::Archive&) const;
      //elle::Status              Extract(elle::Archive&);

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::Natural64           value;

      //
      // static methods
      //
      static elle::Status       Generate(Placement&);
    };

  }
}

# include <XXX/Placement.hxx>

#endif
