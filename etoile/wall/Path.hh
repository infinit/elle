//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue jun 14 12:55:52 2011]
//

#ifndef ETOILE_WALL_PATH_HH
#define ETOILE_WALL_PATH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <etoile/path/Way.hh>
#include <etoile/path/Chemin.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

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

      static elle::Status       Locate(const path::Way&,
                                       path::Way&);
    };

  }
}

#endif
