//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Data.hh
//
// created       julien quintard   [tue aug  4 06:54:28 2009]
// updated       julien quintard   [tue aug  4 17:04:04 2009]
//

#ifndef ETOILE_CORE_DATA_HH
#define ETOILE_CORE_DATA_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/ContentHashBlock.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents file data.
    ///
    class Data:
      public ContentHashBlock
    {
    public:
      //
      // methods
      //
      Status		Read(const Natural64,
			     Byte*,
			     Natural64&) const;
      Status		Write(const Natural64,
			      const Byte*,
			      const Natural64);
      Status		Adjust(const Natural64);

      //
      // interfaces
      //

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
