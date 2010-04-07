//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Route.hh
//
// created       julien quintard   [fri aug  7 22:37:18 2009]
// updated       julien quintard   [wed apr  7 00:49:07 2010]
//

#ifndef ETOILE_PATH_ROUTE_HH
#define ETOILE_PATH_ROUTE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/path/Slice.hh>
#include <etoile/path/Way.hh>

#include <vector>

namespace etoile
{
  namespace path
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a route is a sequence of names forming a path.
    ///
    class Route:
      public Dumpable
    {
    public:
      //
      // types
      //
      typedef std::vector<Slice>	Container;
      typedef Container::iterator	Iterator;
      typedef Container::const_iterator	Scoutor;

      //
      // methods
      //
      Status		Create(const Way&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Container		elements;
    };

  }
}

#endif
