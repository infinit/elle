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
// updated       julien quintard   [mon may  3 19:06:37 2010]
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
  /// XXX \todo virer le code temporaire + mettre chaque valeur dans la
  /// XXX       configuration (root, pair et path)
  ///
  elle::Status		Etoile::Initialize(const hole::Address&	address)
  {
    enter();

    // initialize the configuration.
    if (configuration::Configuration::Initialize() == elle::StatusError)
      escape("unable to initialize the configuration");

    // initialize the kernel.
    if (kernel::Kernel::Initialize() == elle::StatusError)
      escape("unable to initialize the kernel");

    // initialize the depot.
    if (depot::Depot::Initialize() == elle::StatusError)
      escape("unable to initialize the depot");

    // initialize the journal.
    if (journal::Journal::Initialize() == elle::StatusError)
      escape("unable to initialize the journal");

    // initialize the path.
    if (path::Path::Initialize(address) == elle::StatusError)
      escape("unable to initialize the path");

    // initialize the wall.
    if (wall::Wall::Initialize() == elle::StatusError)
      escape("unable to initialize the wall");

    // initialize the user.
    if (user::User::Initialize() == elle::StatusError)
      escape("unable to initialize the user");

    leave();
  }

  ///
  /// this method cleans Etoile.
  ///
  elle::Status		Etoile::Clean()
  {
    enter();

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

    // clean the kernel.
    if (kernel::Kernel::Clean() == elle::StatusError)
      escape("unable to clean the kernel");

    // clean the configuration.
    if (configuration::Configuration::Clean() == elle::StatusError)
      escape("unable to clean the configuration");

    leave();
  }

}
