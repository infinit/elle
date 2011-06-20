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
// updated       julien quintard   [tue jun 14 14:26:18 2011]
//

#ifndef ETOILE_ETOILE_HH
#define ETOILE_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <lune/Lune.hh>

///
/// the principal namespace
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

// XXX #include <etoile/components/Components.hh>
// XXX #include <etoile/configuration/Configuration.hh>
#include <etoile/gear/Gear.hh>
// XXX #include <etoile/depot/Depot.hh>
// XXX #include <etoile/journal/Journal.hh>
// XXX #include <etoile/path/Path.hh>
// XXX #include <etoile/wall/User.hh>
#include <etoile/miscellaneous/Miscellaneous.hh>
#include <etoile/wall/Wall.hh>

//
// ---------- namespace -------------------------------------------------------
//

namespace etoile
{
  //using namespace components;
  //using namespace configuration;
  using namespace gear;
  //using namespace depot;
  //using namespace journal;
  //using namespace path;
  // XXX using namespace user;
  using namespace miscellaneous;
  using namespace wall;
}

#endif
