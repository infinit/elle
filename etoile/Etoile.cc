//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed mar  3 22:36:08 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>
#include <agent/Agent.hh>
#include <hole/Hole.hh>
#include <Infinit.hh>

namespace etoile
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "etoile";

  ///
  /// this variable contains the phrase associated with the Infinit's
  /// current instance.
  ///
  lune::Phrase                  Etoile::Phrase;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes Etoile.
  ///
  elle::Status          Etoile::Initialize()
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

    // initialize the portal.
    if (portal::Portal::Initialize() == elle::StatusError)
      escape("unable to initialize the portal");

    //
    // generate a phrase randomly which will be used by applications to
    // connect to Etoile and trigger specific actions.
    //
    {
      elle::String              string;

      // generate a random string.
      if (elle::Random::Generate(string) == elle::StatusError)
        escape("unable to generate a random string");

      // create the phrase.
      if (Etoile::Phrase.Create(string,
                                portal::Portal::Line) == elle::StatusError)
        escape("unable to create the phrase");

      // store the phrase.
      if (Etoile::Phrase.Store(Infinit::Network) == elle::StatusError)
        escape("unable to store the phrase");
    }

    leave();
  }

  ///
  /// this method cleans Etoile.
  ///
  elle::Status          Etoile::Clean()
  {
    enter();

    // clean the portal.
    if (portal::Portal::Clean() == elle::StatusError)
      escape("unable to clean the portal");

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
