//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Catalog.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [mon apr 19 14:35:47 2010]
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
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constructors & destructors
      //
      Catalog();

      //
      // methods
      //
      Status		Add(Entry*);
      Status		Exist(const path::Slice&);
      Status		Lookup(const path::Slice&,
			       Entry*&);
      Status		Consult(const Index&,
				const Size&,
				Range<Entry>&) const;
      Status		Rename(const path::Slice&,
			       const path::Slice&);
      Status		Remove(const path::Slice&);
      Status		Capacity(Offset&) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, Catalog);
      // XXX operator==

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      State		state;

      Range<Entry>	range;
    };

  }
}

#endif
