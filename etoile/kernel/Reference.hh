//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Reference.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [wed apr  7 00:54:00 2010]
//

#ifndef ETOILE_KERNEL_REFERENCE_HH
#define ETOILE_KERNEL_REFERENCE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/State.hh>
#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Size.hh>

#include <etoile/path/Way.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the contents of a link.
    ///
    class Reference:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // methods
      //
      Status		Bind(const path::Way&);
      Status		Resolve(path::Way&) const;

      Status		Capacity(Size&) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, Reference);
      // XXX operator==

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      path::Way		target;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Length.hh>

#endif
