//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/util/File.hh
//
// created       julien quintard   [sat aug  1 21:06:13 2009]
// updated       julien quintard   [sat aug  1 21:57:53 2009]
//

#ifndef PIG_UTIL_FILE_HH
#define PIG_UTIL_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>

namespace pig
{
  namespace util
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class File
    {
    public:
      //
      // static methods
      //
      static Status	Get(const Address&,
			    File&);
    };

  }
}

#endif
