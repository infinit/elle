//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Identifier.hh
//
// created       julien quintard   [wed mar  3 13:37:54 2010]
// updated       julien quintard   [fri may 28 17:40:25 2010]
//

#ifndef ETOILE_CONCURRENCY_IDENTIFIER_HH
#define ETOILE_CONCURRENCY_IDENTIFIER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// identifiers are used to uniquely identify contexts.
    ///
    class Identifier:
      public elle::Object
    {
    public:
      //
      // constants
      //
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
      elle::Status	Generate();

      //
      // interfaces
      //

      // object
      declare(Identifier);
      elle::Boolean	operator==(const Identifier&) const;
      elle::Boolean	operator<(const Identifier&) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::Natural64	value;
    };

  }
}

#endif
