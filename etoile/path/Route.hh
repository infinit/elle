//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Route.hh
//
// created       julien quintard   [fri aug  7 22:37:18 2009]
// updated       julien quintard   [sat jul 30 17:09:18 2011]
//

#ifndef ETOILE_PATH_ROUTE_HH
#define ETOILE_PATH_ROUTE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/path/Slab.hh>
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
    /// a route is a sequence of sections forming a path each section
    /// representing the name of subdirectory down to the target object along
    /// with their version numbers.
    ///
    /// note that this class also contains the version number of the root
    /// directory.
    ///
    class Route:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Route		Null;

      //
      // types
      //
      typedef std::vector<Slab>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // methods
      //
      elle::Status	Create(const Way&);
      elle::Status	Create(const Route&,
			       const Slab&);

      //
      // interfaces
      //

      // object
      declare(Route);
      elle::Boolean	operator==(const Route&) const;
      elle::Boolean	operator<(const Route&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Container		elements;
    };

  }
}

#endif
