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
// updated       julien quintard   [mon aug 10 16:49:30 2009]
//

#ifndef ETOILE_PATH_PATH_HH
#define ETOILE_PATH_PATH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/components/Components.hh>

#include <etoile/path/Route.hh>

namespace etoile
{
  using namespace core;
  using namespace components;

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
      static Status	Initialize(const Address&);
      static Status	Clean();

      static Status	Resolve(const Route&,
				Address&);

      //
      // static attributes
      //
      static String	Separator;
      static Address	Root;
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
