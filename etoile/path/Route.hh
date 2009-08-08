//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Resolution.hh
//
// created       julien quintard   [fri aug  7 22:37:18 2009]
// updated       julien quintard   [fri aug  7 22:50:19 2009]
//

#ifndef ETOILE_PATH_RESOLUTION_HH
#define ETOILE_PATH_RESOLUTION_HH

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
    /// a resolution contains the addresses corresponding to a path.
    ///
    /// needless to say that a resolution without its corresponding path
    /// is useless.
    ///
    class Resolution
    {
    public:
      //
      // types
      //
      typedef std::vector<Address>	Container;
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
