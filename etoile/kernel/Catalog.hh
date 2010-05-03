//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Catalog.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [mon may  3 22:57:37 2010]
//

#ifndef ETOILE_KERNEL_CATALOG_HH
#define ETOILE_KERNEL_CATALOG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/State.hh>
#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Range.hh>
#include <etoile/kernel/Entry.hh>

#include <etoile/path/Slice.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the content of a directory and is composed
    /// of a set of tuples (name, address).
    ///
    /// \todo XXX note that, for now, all the entries are stored in a single
    /// block until an advanced, multi-block-based, data structure is
    /// developed.
    ///
    class Catalog:
      public elle::Object<>
    {
    public:
      //
      // constructors & destructors
      //
      Catalog();

      //
      // methods
      //
      elle::Status	Add(Entry*);
      elle::Status	Exist(const path::Slice&);
      elle::Status	Lookup(const path::Slice&,
			       Entry*&);
      elle::Status	Consult(const Index&,
				const Size&,
				Range<Entry>&) const;
      elle::Status	Rename(const path::Slice&,
			       const path::Slice&);
      elle::Status	Remove(const path::Slice&);
      elle::Status	Capacity(Offset&) const;

      //
      // interfaces
      //

      // object
      declare(Catalog, _());

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      State		state;

      Range<Entry>	range;
    };

  }
}

#endif
