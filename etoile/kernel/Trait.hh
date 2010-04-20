//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Trait.hh
//
// created       julien quintard   [thu apr  1 19:48:42 2010]
// updated       julien quintard   [tue apr 20 06:29:44 2010]
//

#ifndef ETOILE_KERNEL_TRAIT_HH
#define ETOILE_KERNEL_TRAIT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Subject.hh>
#include <etoile/kernel/Permissions.hh>
#include <etoile/kernel/Token.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an attribute entry i.e a trait.
    ///
    /// a trait is characterised by a string name and while a value string
    /// is associated with it.
    ///
    class Trait:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const Trait		Null;

      //
      // types
      //
      typedef String			S;

      //
      // constructors & destructors
      //
      Trait();
      Trait(const String&,
	    const String&);

      //
      // interfaces
      //

      // entity
      declare(Entity, Trait);
      Boolean		operator==(const Trait&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      // rangeable
      String&		Symbol();

      //
      // attributes
      //
      String		name;
      String		value;
    };

  }
}

#endif
