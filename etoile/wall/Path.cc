//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Path.cc
//
// created       julien quintard   [tue jun 14 12:57:24 2011]
// updated       julien quintard   [wed aug  3 23:41:42 2011]
//
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Path.hh>

#include <etoile/path/Path.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes a string-based path i.e a way and resolves it
    /// into a location.
    ///
    /// note that the way may contain version indicators for Etoile to
    /// use a specific version of the named directory, file etc. contained
    /// in the path.
    ///
    elle::Status	Path::Resolve(
			  const path::Way&			way,
			  path::Chemin&				chemin)
    {
      path::Route	route;
      path::Venue	venue;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Path::Resolve()\n");

      // create a route from the way.
      if (route.Create(way) == elle::StatusError)
	escape("unable to create the route");

      // resolve the way.
      if (path::Path::Resolve(route, venue) == elle::StatusError)
	escape("unable to resolve the path");

      // create the chemin.
      if (chemin.Create(route, venue) == elle::StatusError)
	escape("unable to create the chemin");

      leave();
    }

  }
}
