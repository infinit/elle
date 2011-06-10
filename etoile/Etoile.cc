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
// updated       julien quintard   [fri jun 10 13:28:44 2011]
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

    /* XXX
    // initialize the configuration.
    if (configuration::Configuration::Initialize() == elle::StatusError)
      escape("unable to initialize the configuration");

    // initialize the depot.
    if (depot::Depot::Initialize() == elle::StatusError)
      escape("unable to initialize the depot");

    // initialize the journal.
    if (journal::Journal::Initialize() == elle::StatusError)
      escape("unable to initialize the journal");

    // initialize the path.
    if (path::Path::Initialize() == elle::StatusError)
      escape("unable to initialize the path");

    // initialize the wall.
    if (wall::Wall::Initialize() == elle::StatusError)
      escape("unable to initialize the wall");

    // initialize the user.
    if (user::User::Initialize() == elle::StatusError)
      escape("unable to initialize the user");
    */

    leave();
  }

  ///
  /// this method cleans Etoile.
  ///
  elle::Status		Etoile::Clean()
  {
    enter();

    /* XXX
    // clean the user.
    if (user::User::Clean() == elle::StatusError)
      escape("unable to clean the user");

    // clean the wall.
    if (wall::Wall::Clean() == elle::StatusError)
      escape("unable to clean the wall");

    // clean the path.
    if (path::Path::Clean() == elle::StatusError)
      escape("unable to clean the path");

    // clean the journal.
    if (journal::Journal::Clean() == elle::StatusError)
      escape("unable to clean the journal");

    // clean the depot.
    if (depot::Depot::Clean() == elle::StatusError)
      escape("unable to clean the depot");

    // clean the configuration.
    if (configuration::Configuration::Clean() == elle::StatusError)
      escape("unable to clean the configuration");
    */

    leave();
  }

}
