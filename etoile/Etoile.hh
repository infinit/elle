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
// updated       julien quintard   [thu may  5 16:52:02 2011]
//

#ifndef ETOILE_ETOILE_HH
#define ETOILE_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh> // XXX a virer
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
    static elle::Status	Initialize(const nucleus::Address& =
				   nucleus::Address::Null);
    static elle::Status	Clean();
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Components.hh>
#include <etoile/configuration/Configuration.hh>
#include <etoile/context/Context.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/hole/Hole.hh>
#include <etoile/journal/Journal.hh>
#include <etoile/path/Path.hh>
#include <etoile/wall/User.hh>
#include <etoile/wall/Wall.hh>

//
// ---------- namespace -------------------------------------------------------
//

namespace etoile
{
  using namespace components;
  using namespace configuration;
  using namespace context;
  using namespace depot;
  using namespace hole;
  using namespace journal;
  using namespace path;
  using namespace user;
  using namespace wall;
}

#endif
