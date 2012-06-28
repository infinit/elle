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
  if (path::Path::Initialize() == elle::Status::Error)
    escape("unable to initialize the path");

  if (gear::Gear::Initialize() == elle::Status::Error)
    escape("unable to initialize the gear");

  if (shrub::Shrub::Initialize() == elle::Status::Error)
    escape("unable to initialize the shrub");

  if (portal::Portal::Initialize() == elle::Status::Error)
    escape("unable to initialize the portal");

  return elle::Status::Ok;
}

///
/// this method cleans Etoile.
///
elle::Status          Etoile::Clean()
{
  if (portal::Portal::Clean() == elle::Status::Error)
    escape("unable to clean the portal");

  if (shrub::Shrub::Clean() == elle::Status::Error)
    escape("unable to clean the shrub");

  if (gear::Gear::Clean() == elle::Status::Error)
    escape("unable to clean the gear");

  if (path::Path::Clean() == elle::Status::Error)
    escape("unable to clean the path");

  return elle::Status::Ok;
}
