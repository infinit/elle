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
// updated       julien quintard   [mon may  3 12:25:31 2010]
//

#ifndef ETOILE_ETOILE_HH
#define ETOILE_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <lune/Lune.hh>

#include <etoile/hole/Address.hh> // XXX a virer des qu'on aurait les memento

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
    static elle::Status	Initialize(const hole::Address& = hole::Address::Null);
    static elle::Status	Clean();
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

//
// ---------- namespace -------------------------------------------------------
//

namespace etoile
{
  using namespace components;
  using namespace configuration;
  using namespace context;
  using namespace kernel;
  using namespace depot;
  using namespace hole;
  using namespace journal;
  using namespace path;
  using namespace user;
  using namespace wall;
}

#endif
