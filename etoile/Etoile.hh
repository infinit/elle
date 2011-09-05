//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/Etoile.hh
//
// created       julien quintard   [mon feb 16 19:12:33 2009]
// updated       julien quintard   [sun sep  4 20:54:15 2011]
//

#ifndef ETOILE_ETOILE_HH
#define ETOILE_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <lune/Lune.hh>

///
/// the principal Etoile namespace which provides functionalities for
/// manipulating the file system objects.
///
namespace etoile
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class controls Etoile.
  ///
  class Etoile
  {
  public:
    //
    // static methods
    //
    static elle::Status	Initialize();
    static elle::Status	Clean();
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Depot.hh>
#include <etoile/gear/Gear.hh>
#include <etoile/journal/Journal.hh>
#include <etoile/miscellaneous/Miscellaneous.hh>
#include <etoile/path/Path.hh>
#include <etoile/shrub/Shrub.hh>
#include <etoile/wall/Wall.hh>

//
// ---------- namespace -------------------------------------------------------
//

namespace etoile
{
  using namespace depot;
  using namespace gear;
  using namespace journal;
  using namespace miscellaneous;
  using namespace path;
  using namespace shrub;
  using namespace wall;
}

#endif
