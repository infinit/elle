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
// updated       julien quintard   [mon aug  3 20:41:05 2009]
//

#ifndef PIG_UTIL_PATH_HH
#define PIG_UTIL_PATH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

#include <pig/cache/Cache.hh>
#include <pig/hole/Hole.hh>
#include <pig/util/Directory.hh>

#include <vector>

namespace pig
{
  using namespace cache;
  using namespace hole;

  namespace util
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
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
			      String&,
			      std::list<String>&);
      static Status	Break(const String&,
			      const String&,
			      String&,
			      String&);
    };

  }
}

#endif
