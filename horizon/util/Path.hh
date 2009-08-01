//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/util/Path.hh
//
// created       julien quintard   [sat aug  1 21:24:57 2009]
// updated       julien quintard   [sat aug  1 22:57:56 2009]
//

#ifndef PIG_UTIL_PATH_HH
#define PIG_UTIL_PATH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

#include <vector>

namespace pig
{
  namespace util
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Path
    {
    public:
      //
      // static methods
      //
      static Status	Resolve(const String&,
				Address&);
      static Status	Split(const String&,
			      const String&,
			      std::vector<String>&);
      static Status	Merge(const std::vector<String>&,
			      const Natural32,
			      String&);
    };

  }
}

#endif
