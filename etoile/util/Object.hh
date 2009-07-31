//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/util/Object.hh
//
// created       julien quintard   [fri jul 31 00:33:09 2009]
// updated       julien quintard   [fri jul 31 00:35:00 2009]
//

#ifndef ETOILE_UTIL_OBJECT_HH
#define ETOILE_UTIL_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Core.hh>

namespace etoile
{
  namespace util
  {

    class Object
    {
      //
      // static methods
      //
      static Status	Get(const Address&,
			    Object&);
    };

  }
}

#endif
