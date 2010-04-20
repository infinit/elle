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
// updated       julien quintard   [tue apr 20 10:22:04 2010]
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
      // constructors & destructors
      //
      Data();

      //
      // methods
      //
      Status		Write(const Offset&,
			      const Region&);
      Status		Read(const Offset&,
			     const Size&,
			     Region&) const;
      Status		Adjust(const Size&);

      Status		Capacity(Size&) const;

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
      State		state;

      Region		region;
    };

  }
}

#endif
