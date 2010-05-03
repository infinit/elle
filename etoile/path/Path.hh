//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Path.hh
//
// created       julien quintard   [fri aug  7 22:37:18 2009]
// updated       julien quintard   [mon may  3 13:28:16 2010]
//

#ifndef ETOILE_PATH_PATH_HH
#define ETOILE_PATH_PATH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>

#include <etoile/path/Route.hh>

namespace etoile
{
  ///
  /// this namespace contains everything used for resolving paths into
  /// block addresses.
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
      static elle::Status	Initialize(const hole::Address&);
      static elle::Status	Clean();

      static elle::Status	Resolve(const Route&,
					hole::Address&);

      //
      // static attributes
      //
      static hole::Address*	Root;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Cache.hh>
#include <etoile/path/Item.hh>
#include <etoile/path/Length.hh>
#include <etoile/path/Route.hh>
#include <etoile/path/Slice.hh>
#include <etoile/path/Venue.hh>
#include <etoile/path/Way.hh>

#endif
