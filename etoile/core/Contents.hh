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
// updated       julien quintard   [fri aug 21 22:01:22 2009]
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
    };

  }
}

#endif
