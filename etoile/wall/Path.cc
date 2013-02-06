#include <etoile/wall/Path.hh>
#include <etoile/path/Path.hh>
#include <etoile/path/Way.hh>
#include <etoile/path/Chemin.hh>

#include <elle/log.hh>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.etoile.wall.Path");

namespace etoile
{
  namespace wall
  {
    path::Chemin
    Path::resolve(const path::Way& way)
    {
      ELLE_TRACE_FUNCTION(way);

      path::Route       route;
      path::Venue       venue;

      // Create a route from the way.
      if (route.Create(way) == elle::Status::Error)
        throw reactor::Exception("unable to create the route");

      // Resolve the route.
      if (path::Path::Resolve(route, venue) == elle::Status::Error)
        throw reactor::Exception("unable to resolve the route");

      // Create the chemin.
      path::Chemin chemin;
      if (chemin.Create(route, venue) == elle::Status::Error)
        throw reactor::Exception("unable to create the chemin");
      return chemin;
    }

    // XXX[to move somewhere else]
    NoSuchFileOrDirectory::NoSuchFileOrDirectory(reactor::Scheduler& sched,
                                                 path::Way const& path):
      reactor::Exception(elle::sprintf("no such file or directory: %s", path)),
      _path(path)
    {}

    // XXX[to move somewhere else]
    NoSuchFileOrDirectory::~NoSuchFileOrDirectory() throw ()
    {}
  }
}
