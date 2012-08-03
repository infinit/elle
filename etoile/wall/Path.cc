#include <etoile/wall/Path.hh>
#include <etoile/path/Path.hh>
#include <etoile/path/Way.hh>
#include <etoile/path/Chemin.hh>

#include <elle/log.hh>

#include <Infinit.hh>

ELLE_LOG_TRACE_COMPONENT("etoile.wall.Path");

namespace etoile
{
  namespace wall
  {

    ///
    /// this method takes a string-based path i.e a way and resolves it
    /// into a location.
    ///
    /// note that the way may contain version indicators for Etoile to
    /// use a specific version of the named directory, file etc. contained
    /// in the path.
    ///
    elle::Status        Path::Resolve(
                          const path::Way&                      way,
                          path::Chemin&                         chemin)
    {
      path::Route       route;
      path::Venue       venue;

      ELLE_LOG_TRACE_SCOPE("Resolve(%s)", way.path.c_str());

      // create a route from the way.
      if (route.Create(way) == elle::Status::Error)
        escape("unable to create the route");

      // resolve the route.
      if (path::Path::Resolve(route, venue) == elle::Status::Error)
        escape("unable to resolve the route");

      // create the chemin.
      if (chemin.Create(route, venue) == elle::Status::Error)
        escape("unable to create the chemin");

      return elle::Status::Ok;
    }

  }
}
