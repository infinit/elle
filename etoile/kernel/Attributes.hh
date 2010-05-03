//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Attributes.hh
//
// created       julien quintard   [wed mar 31 23:32:06 2010]
// updated       julien quintard   [mon may  3 22:56:08 2010]
//

#ifndef ETOILE_KERNEL_ATTRIBUTES_HH
#define ETOILE_KERNEL_ATTRIBUTES_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Trait.hh>
#include <etoile/kernel/Index.hh>
#include <etoile/kernel/Size.hh>
#include <etoile/kernel/Range.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains a list of (key, data) tuples that can be
    /// used for storing object-specific extra information.
    ///
    class Attributes:
      public elle::Object<>
    {
    public:
      //
      // methods
      //
      elle::Status	Add(Trait*);
      elle::Status	Exist(const elle::String&);
      elle::Status	Lookup(const elle::String&,
			       Trait*&);
      elle::Status	Consult(const Index&,
				const Size&,
				Range<Trait>&) const;
      elle::Status	Update(const elle::String&,
			       const elle::String&);
      elle::Status	Remove(const elle::String&);
      elle::Status	Capacity(Size&) const;

      //
      // interfaces
      //

      // object
      declare(Attributes, _());

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Range<Trait>	range;
    };

  }
}

#endif
