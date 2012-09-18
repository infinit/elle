#ifndef  ETOILE_WALL_OBJECT_HXX
# define ETOILE_WALL_OBJECT_HXX

# include <etoile/gear/Scope.hh>
# include <etoile/shrub/Shrub.hh>
# include <etoile/path/Path.hh>
# include <etoile/path/Chemin.hh>

# include <elle/concurrency/Scheduler.hh>

namespace etoile
{
  namespace wall
  {

      template <typename T>
      void
      Object::reload(gear::Scope& scope)
      {
        ELLE_LOG_COMPONENT("etoile.wall.Object");
        ELLE_TRACE_SCOPE("reload(%s)", scope);

        ELLE_TRACE("remove a dirty block %s", scope.chemin.route)
          shrub::Shrub::clear();

        ELLE_TRACE("try to resolve the route now that the cache was cleaned")
        {
          path::Venue venue;
          if (path::Path::Resolve(scope.chemin.route,
                                  venue) == elle::Status::Error)
            throw reactor::Exception{
                elle::concurrency::scheduler(),
                elle::sprintf("unable to resolve the route %s",
                              scope.chemin.route)
            };
          scope.chemin = path::Chemin(scope.chemin.route, venue);
        }

        ELLE_TRACE("Route was successfully resolved into %s, loading object",
                   scope.chemin.route)
        {
          T* context = nullptr;
          if (scope.Use(context) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     "unable to use the context");

          if (T::A::Load(*context) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     "unable to load the object");
        }
      }

  }
}

#endif
