//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Path.hh
//
// created       julien quintard   [fri aug  7 22:37:18 2009]
// updated       julien quintard   [fri aug  7 22:51:12 2009]
//

#ifndef ETOILE_PATH_PATH_HH
#define ETOILE_PATH_PATH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <vector>

namespace etoile
{
  namespace path
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a path is represented by a sequence of names.
    ///
    class Path
    {
    public:
      //
      // types
      //
      typedef std::vector<String>	Container;
      typedef Container::iterator	Iterator;
      typedef Container::const_iterator	Explorer;

      //
      // attributes
      //
      Container		elements;
    };

  }
}

#endif
