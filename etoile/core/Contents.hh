//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Contents.hh
//
// created       julien quintard   [mon aug 10 12:07:15 2009]
// updated       julien quintard   [wed dec 16 17:20:55 2009]
//

#ifndef ETOILE_CORE_CONTENTS_HH
#define ETOILE_CORE_CONTENTS_HH

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
    /// this classes abstracts the data contained in an object.
    ///
    class Contents:
      public ContentHashBlock
    {
    public:
      //
      // types
      //
      typedef Natural64		Offset;

      //
      // methods
      //
      virtual Status	Size(Natural64&) const = 0;
    };

  }
}

#endif
