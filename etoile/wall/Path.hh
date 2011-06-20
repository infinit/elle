//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Path.hh
//
// created       julien quintard   [tue jun 14 12:55:52 2011]
// updated       julien quintard   [thu jun 16 10:11:00 2011]
//

#ifndef ETOILE_WALL_PATH_HH
#define ETOILE_WALL_PATH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
      static elle::Status	Resolve(const path::Way&,
					path::Chemin&);
    };

  }
}

#endif
