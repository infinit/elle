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
// updated       julien quintard   [sat mar 27 00:45:04 2010]
//

#ifndef ETOILE_ETOILE_HH
#define ETOILE_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
    static Status	Initialize();
    static Status	Clean();
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Components.hh>
#include <etoile/configuration/Configuration.hh>
#include <etoile/context/Context.hh>
#include <etoile/kernel/Kernel.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/hole/Hole.hh>
#include <etoile/journal/Journal.hh>
#include <etoile/path/Path.hh>
#include <etoile/wall/User.hh>
#include <etoile/wall/Wall.hh>

using namespace etoile;
using namespace etoile::components;
using namespace etoile::configuration;
using namespace etoile::context;
using namespace etoile::kernel;
using namespace etoile::depot;
using namespace etoile::hole;
using namespace etoile::journal;
using namespace etoile::path;
using namespace etoile::user;
using namespace etoile::wall;

#endif
