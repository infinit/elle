#ifndef ETOILE_WALL_OBJECT_HH
# define ETOILE_WALL_OBJECT_HH

# include <elle/types.hh>
# include <elle/log.hh>

# include <etoile/path/fwd.hh>
# include <etoile/gear/fwd.hh>
# include <etoile/abstract/fwd.hh>

namespace etoile
{
  namespace wall
  {

    ///
    /// this class provides general-purpose methods for manipulating objects.
    ///
    /// these are very useful when the caller does not know the genre of the
    /// object i.e file, directory or link.
    ///
    class Object
    {
    public:
      /// Loads the objec identified by \a chemin into a new scope and return
      /// the scope's identifier.
      static
      gear::Identifier
      load(path::Chemin const& chemin);
      /// The general abstract regarding the identified object.
      static
      abstract::Object
      information(gear::Identifier const& identifier);
      /// Discard the scope, potentially ignoring some modifications.
      static
      void
      discard(gear::Identifier const&);
      /// Commit the pending modifications by placing the scope in the journal.
      static
      void
      store(gear::Identifier const& identifier);
      /// Destroy an object.
      ///
      /// Use with great care since, not knowing the object's genre, the data
      /// blocks will not be removed. therefore, the genre-specific Destroy()
      /// method should always be preferred.
      static
      void
      destroy(gear::Identifier const&);
      /// Remove all the blocks of all the versions associated with an
      /// object.
      ///
      /// Use with great care since, not knowing the object's genre, the data
      /// blocks will not be removed. therefore, the genre-specific Destroy()
      /// method should always be preferred.
      static
      void
      purge(gear::Identifier const&);
      /// Remove the route entry from the cache (shrub) and try to reload the
      /// Context of type T.
      template <typename T>
      static
      void
      reload(gear::Scope& scope);
    };

  }
}

# include "Object.hxx"

#endif
