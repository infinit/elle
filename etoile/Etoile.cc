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

using namespace etoile;

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
lune::Phrase Etoile::Phrase;

//
// ---------- methods ---------------------------------------------------------
//

///
/// this method initializes Etoile.
///
elle::Status          Etoile::Initialize()
{
  //
  // initialize the components.
  //
  {
    if (path::Path::Initialize() == elle::Status::Error)
      escape("unable to initialize the path");

    if (depot::Depot::Initialize() == elle::Status::Error)
      escape("unable to initialize the depot");

    if (gear::Gear::Initialize() == elle::Status::Error)
      escape("unable to initialize the gear");

    if (shrub::Shrub::Initialize() == elle::Status::Error)
      escape("unable to initialize the shrub");

    if (portal::Portal::Initialize() == elle::Status::Error)
      escape("unable to initialize the portal");
  }

  //
  // generate a phrase randomly which will be used by applications to
  // connect to Etoile and trigger specific actions.
  //
  // generate a random string, create a phrase with it along with
  // the socket used by portal so that applications have everything
  // to connect to and authenticate to portal.
  {
    elle::String string;

    if (elle::Random::Generate(string) == elle::Status::Error)
      escape("unable to generate a random string");

    if (Etoile::Phrase.Create(string,
                              portal::Portal::Line) == elle::Status::Error)
      escape("unable to create the phrase");

    if (Etoile::Phrase.Store(Infinit::Network) == elle::Status::Error)
      escape("unable to store the phrase");
  }

  return elle::Status::Ok;
}

///
/// this method cleans Etoile.
///
elle::Status          Etoile::Clean()
{
  //
  // delete the phrase.
  //
  {
    if (Etoile::Phrase.Erase(Infinit::Network) == elle::Status::Error)
      escape("unable to erase the phrase");
  }

  //
  // clean the components.
  //
  {
    if (portal::Portal::Clean() == elle::Status::Error)
      escape("unable to clean the portal");

    if (shrub::Shrub::Clean() == elle::Status::Error)
      escape("unable to clean the shrub");

    if (gear::Gear::Clean() == elle::Status::Error)
      escape("unable to clean the gear");

    if (depot::Depot::Clean() == elle::Status::Error)
      escape("unable to clean the depot");

    if (path::Path::Clean() == elle::Status::Error)
      escape("unable to clean the path");
  }

  return elle::Status::Ok;
}
