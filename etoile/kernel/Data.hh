//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Data.hh
//
// created       julien quintard   [tue aug  4 06:54:28 2009]
// updated       julien quintard   [fri apr  2 11:28:30 2010]
//

#ifndef ETOILE_KERNEL_DATA_HH
#define ETOILE_KERNEL_DATA_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/State.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents file data.
    ///
    class Data:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // methods
      //
      Status		Read(const Offset&,
			     Byte*,
			     Size&) const;
      Status		Write(const Offset&,
			      const Byte*,
			      const Size&);
      Status		Adjust(const Size&);

      Status		Capacity(Offset&) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, Data);
      // XXX operator==

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Region		region;
    };

  }
}

#endif
