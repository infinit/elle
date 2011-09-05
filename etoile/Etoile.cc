//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/Etoile.cc
//
// created       julien quintard   [wed mar  3 22:36:08 2010]
// updated       julien quintard   [sun sep  4 13:48:18 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>

namespace etoile
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "etoile";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes Etoile.
  ///
  elle::Status		Etoile::Initialize()
  {
    enter();

    // initialize the path.
    if (path::Path::Initialize() == elle::StatusError)
      escape("unable to initialize the path");

    // initialize the depot.
    if (depot::Depot::Initialize() == elle::StatusError)
      escape("unable to initialize the depot");

    // initialize the gear.
    if (gear::Gear::Initialize() == elle::StatusError)
      escape("unable to initialize the gear");

    // initialize the shrub.
    if (shrub::Shrub::Initialize() == elle::StatusError)
      escape("unable to initialize the shrub");

    leave();
  }

  ///
  /// this method cleans Etoile.
  ///
  elle::Status		Etoile::Clean()
  {
    enter();

    // clean the shrub.
    if (shrub::Shrub::Clean() == elle::StatusError)
      escape("unable to clean the shrub");

    // clean the gear.
    if (gear::Gear::Clean() == elle::StatusError)
      escape("unable to clean the gear");

    // clean the depot.
    if (depot::Depot::Clean() == elle::StatusError)
      escape("unable to clean the depot");

    // clean the path.
    if (path::Path::Clean() == elle::StatusError)
      escape("unable to clean the path");

    leave();
  }

}
