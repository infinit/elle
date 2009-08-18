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
// updated       julien quintard   [sat aug 15 00:20:27 2009]
//

#ifndef ETOILE_PATH_PATH_HH
#define ETOILE_PATH_PATH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/components/Components.hh>
#include <etoile/hole/Hole.hh>

#include <etoile/path/Route.hh>

namespace etoile
{
  namespace path
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
      static Status	Initialize(const hole::Address&);
      static Status	Clean();

      static Status	Resolve(const Route&,
				hole::Address&);

      //
      // static attributes
      //
      static String		Separator;
      static hole::Address	Root;
    };

  }
}

// XXX
#include <etoile/path/Cache.hh>
#include <etoile/path/Item.hh>
#include <etoile/path/Route.hh>
#include <etoile/path/Venue.hh>
// XXX

#endif
