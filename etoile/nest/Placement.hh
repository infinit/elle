//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon oct 17 10:52:09 2011]
//

#ifndef ETOILE_NEST_PLACEMENT_HH
#define ETOILE_NEST_PLACEMENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace nest
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class can be used to uniquely identify actors whose role is
    /// to act on scopes.
    ///
    class Placement:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const elle::Natural64	Zero;

      static const Placement		Null;

      //
      // static attribute
      //
      static elle::Natural64		Counter;

      //
      // constructors & destructors
      //
      Placement();

      //
      // methods
      //
      elle::Status		Generate();

      //
      // interfaces
      //

      // object
      declare(Placement);
      elle::Boolean		operator==(const Placement&) const;
      elle::Boolean		operator<(const Placement&) const;

      // archivable
      elle::Status		Serialize(elle::Archive&) const;
      elle::Status		Extract(elle::Archive&);

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::Natural64		value;
    };

  }
}

#endif
