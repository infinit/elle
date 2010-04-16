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
// updated       julien quintard   [wed apr 14 14:57:10 2010]
//

#ifndef ETOILE_KERNEL_CATALOG_HH
#define ETOILE_KERNEL_CATALOG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/State.hh>
#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Set.hh>

#include <etoile/path/Slice.hh>

#include <list>

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
				Set&) const;
      Status		Remove(const path::Slice&);
      Status		Rename(const path::Slice&,
			       const path::Slice&);
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

      Set		set;
    };

  }
}

#endif
