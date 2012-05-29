#include <Infinit.hh>

#include <elle/cryptography/Random.hh>

#include <etoile/Etoile.hh>
#include <agent/Agent.hh>
#include <etoile/Etoile.hh>
#include <hole/Hole.hh>

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

    if (gear::Gear::Initialize() == elle::StatusError)
      escape("unable to initialize the gear");

    if (shrub::Shrub::Initialize() == elle::Status::Error)
      escape("unable to initialize the shrub");

    // XXX
    // if (portal::Portal::Initialize() == elle::StatusError)
    //   escape("unable to initialize the portal");
  }

  //
  // generate a phrase randomly which will be used by applications to
  // connect to Etoile and trigger specific actions.
  //
  // generate a random string, create a phrase with it along with
  // the socket used by portal so that applications have everything
  // to connect to and authenticate to portal.
  if (!Infinit::Network.empty())
    {
      elle::String string;

      if (elle::cryptography::Random::Generate(string) == elle::Status::Error)
        escape("unable to generate a random string");

    // if (Etoile::Phrase.Create(string,
    //                           portal::Portal::Line) == elle::StatusError)
    //   escape("unable to create the phrase");

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
  if (!Infinit::Network.empty() &&
      Etoile::Phrase.Erase(Infinit::Network) == elle::Status::Error)
      escape("unable to erase the phrase");

  //
  // clean the components.
  //
  {
    // XXX
    // if (portal::Portal::Clean() == elle::StatusError)
    //   escape("unable to clean the portal");

    if (shrub::Shrub::Clean() == elle::Status::Error)
      escape("unable to clean the shrub");

    if (gear::Gear::Clean() == elle::Status::Error)
      escape("unable to clean the gear");

    if (path::Path::Clean() == elle::StatusError)
      escape("unable to clean the path");
  }

  return elle::Status::Ok;
}
