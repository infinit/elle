#ifndef ETOILE_WALL_PATH_HH
# define ETOILE_WALL_PATH_HH

# include <elle/types.hh>

# include <etoile/path/fwd.hh>

namespace etoile
{
  namespace wall
  {

    ///
    /// this class provides methods for resolving paths into locations.
    ///
    class Path
    {
    public:
      //
      // static methods
      //
      static elle::Status       Resolve(const path::Way&,
                                        path::Chemin&);
    };

  }
}

#endif
