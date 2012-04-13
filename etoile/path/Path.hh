//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug  7 22:37:18 2009]
//

#ifndef ETOILE_PATH_PATH_HH
#define ETOILE_PATH_PATH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Route.hh>
#include <etoile/path/Venue.hh>

namespace etoile
{
  ///
  /// this namespace contains everything necessary for resolving paths.
  ///
  namespace path
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is the entry point to path resolution.
    ///
    class Path
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Clean();

      static elle::Status       Resolve(const Route&,
                                        Venue&);
      static elle::Status       Parse(const Slab&,
                                      Slice&,
                                      nucleus::Version&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Chemin.hh>
#include <etoile/path/Length.hh>
#include <etoile/path/Route.hh>
#include <etoile/path/Slab.hh>
#include <etoile/path/Slice.hh>
#include <etoile/path/Venue.hh>
#include <etoile/path/Way.hh>

#endif
