//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Identifier.hh
//
// created       julien quintard   [wed mar  3 13:37:54 2010]
// updated       julien quintard   [sun sep  4 15:06:34 2011]
//

#ifndef ETOILE_GEAR_IDENTIFIER_HH
#define ETOILE_GEAR_IDENTIFIER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class can be used to uniquely identify actors whose role is
    /// to act on scopes.
    ///
    class Identifier:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const elle::Natural64	Zero;

      static const Identifier		Null;

      //
      // static attribute
      //
      static elle::Natural64		Counter;

      //
      // constructors & destructors
      //
      Identifier();

      //
      // methods
      //
      elle::Status		Generate();

      //
      // interfaces
      //

      // object
      declare(Identifier);
      elle::Boolean		operator==(const Identifier&) const;
      elle::Boolean		operator<(const Identifier&) const;

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
