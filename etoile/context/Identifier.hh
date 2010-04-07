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
// updated       julien quintard   [wed mar 31 01:17:13 2010]
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
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const Identifier		Null;

      //
      // static attribute
      //
      static Natural64			Counter;

      //
      // constructors & destructors
      //
      Identifier();

      //
      // methods
      //
      Status		Generate();

      //
      // interfaces
      //

      // entity
      declare(Entity, Identifier);
      Boolean		operator==(const Identifier&) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Natural64		value;
    };

//
// ---------- operators -------------------------------------------------------
//

    Boolean		operator<(const Identifier&,
				  const Identifier&);

  }
}

#endif
