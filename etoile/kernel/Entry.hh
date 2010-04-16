//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Entry.hh
//
// created       julien quintard   [fri apr  2 00:03:02 2010]
// updated       julien quintard   [thu apr 15 15:24:12 2010]
//

#ifndef ETOILE_KERNEL_ENTRY_HH
#define ETOILE_KERNEL_ENTRY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>

#include <etoile/path/Slice.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a directory i.e catalog entry which is
    /// composed of a name and its object's corresponding address.
    ///
    class Entry:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const Entry		Null;

      //
      // constructors & destructors
      //
      Entry();
      Entry(const path::Slice&,
	    const hole::Address&);

      //
      // interfaces
      //

      // entity
      declare(Entity, Entry);
      Boolean		operator==(const Entry&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      path::Slice	name;
      hole::Address	address;
    };

  }
}

#endif
