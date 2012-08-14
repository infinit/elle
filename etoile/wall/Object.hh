#ifndef ETOILE_WALL_OBJECT_HH
# define ETOILE_WALL_OBJECT_HH

# include <elle/types.hh>

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
    /// these are very useful when the caller does not know the genre of
    /// the object i.e file, directory or link.
    ///
    class Object
    {
    public:
      //
      // static methods
      //
      static elle::Status       Load(const path::Chemin&,
                                     gear::Identifier&);

      static elle::Status       Lock(const gear::Identifier&);
      static elle::Status       Release(const gear::Identifier&);
      static elle::Status       Information(const gear::Identifier&,
                                            abstract::Object&);

      static elle::Status       Discard(const gear::Identifier&);
      static elle::Status       Store(const gear::Identifier&);
      static elle::Status       Destroy(const gear::Identifier&);
      static elle::Status       Purge(const gear::Identifier&);
    };

  }
}

#endif
