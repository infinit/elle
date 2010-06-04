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
// updated       julien quintard   [fri may 28 17:47:53 2010]
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
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Entry		Null;

      //
      // types
      //
      typedef path::Slice		S;

      //
      // constructors & destructors
      //
      Entry();
      Entry(const path::Slice&,
	    const hole::Address&);

      //
      // interfaces
      //

      // object
      declare(Entry);
      elle::Boolean	operator==(const Entry&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      // rangeable
      path::Slice&	Symbol();

      //
      // attributes
      //
      path::Slice	name;
      hole::Address	address;
    };

  }
}

#endif
