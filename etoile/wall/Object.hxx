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

        ELLE_TRACE("clearing the cache in order to evict %s", scope.chemin.route)
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

        ELLE_DEBUG("route was successfully resolved into %s",
                   scope.chemin.route);

        ELLE_TRACE("loading object ");
        {
          T* context = nullptr;
          if (scope.Use(context) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     "unable to use the context");

          // Reset location
          nucleus::proton::Location location;
          if (scope.chemin.Locate(location) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                "unable to locate the object");

          context->location = location;
          // XXX We force the loading.
          delete context->object;
          context->object = nullptr;
          context->state = gear::Context::StateUnknown;

          if (T::A::Load(*context) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     "unable to load the object");
        }
      }

  }
}

#endif
