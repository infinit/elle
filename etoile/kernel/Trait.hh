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
// updated       julien quintard   [fri may 28 17:46:53 2010]
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
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Trait		Null;

      //
      // types
      //
      typedef elle::String		S;

      //
      // constructors & destructors
      //
      Trait();
      Trait(const elle::String&,
	    const elle::String&);

      //
      // interfaces
      //

      // object
      declare(Trait);
      elle::Boolean	operator==(const Trait&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      // rangeable
      elle::String&	Symbol();

      //
      // attributes
      //
      elle::String	name;
      elle::String	value;
    };

  }
}

#endif
