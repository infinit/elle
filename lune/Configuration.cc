//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [sun jun 19 23:19:22 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/Fileable.hxx>
#include <elle/utility/SettingsSerializer.hxx>

#include <lune/Configuration.hh>

#include <lune/Lune.hh>
#include <horizon/Horizon.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the configuration files extension.
  ///
  const elle::String    Configuration::Extension = ".conf";

  ///
  /// etoile-specific configuration values.
  ///
  const elle::Boolean
  Configuration::Default::Etoile::Debug = false;

  const elle::Natural32
  Configuration::Default::Etoile::Gear::Containment = 2000;

  const elle::Boolean
  Configuration::Default::Etoile::Shrub::Status = true;
  const elle::Natural32
  Configuration::Default::Etoile::Shrub::Capacity = 1024;
  const elle::Natural32
  Configuration::Default::Etoile::Shrub::Lifespan = 300;
  const elle::Natural32
  Configuration::Default::Etoile::Shrub::Frequency = 120000;

  const elle::Boolean
  Configuration::Default::Etoile::History::Status = false;
  const elle::Character
  Configuration::Default::Etoile::History::Indicator::Root = '@';
  const elle::Character
  Configuration::Default::Etoile::History::Indicator::Slab = '%';

  ///
  /// nucleus-specific configuration values.
  ///
  const elle::Boolean
  Configuration::Default::Nucleus::Debug = false;

  ///
  /// hole-specific configuration values.
  ///
  const elle::Boolean
  Configuration::Default::Hole::Debug = false;

  ///
  /// horizon-specific configuration values.
  ///
  const elle::Boolean
  Configuration::Default::Horizon::Debug = false;

  ///
  /// satellites-specific configuration values.
  ///
  const elle::Boolean
  Configuration::Default::Satellites::Debug = false;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method synchronises the in-memory configuration so as to
  /// be stored.
  ///
  elle::Status          Configuration::Push()
  {
    //
    // etoile
    //
    (*this)["etoile"].Set("debug", this->etoile.debug);
    (*this)["etoile"].Set("gear.containment", this->etoile.gear.containment);
    (*this)["etoile"].Set("shrub.status", this->etoile.shrub.status);
    (*this)["etoile"].Set("shrub.capacity", this->etoile.shrub.capacity);
    (*this)["etoile"].Set("shrub.lifespan", this->etoile.shrub.lifespan);
    (*this)["etoile"].Set("shrub.frequency", this->etoile.shrub.frequency);
    (*this)["etoile"].Set("history.status", this->etoile.history.status);
    (*this)["etoile"].Set("history.indicator.root",
                          this->etoile.history.indicator.root);
    (*this)["etoile"].Set("history.indicator.slab",
                          this->etoile.history.indicator.slab);

    //
    // nucleus
    //
    (*this)["nucleus"].Set("debug", this->nucleus.debug);

    //
    // hole
    //
    (*this)["hole"].Set("debug", this->hole.debug);

    //
    // horizon
    //
    (*this)["horizon"].Set("debug", this->horizon.debug);

    //
    // satellites
    //
    (*this)["satellites"].Set("debug", this->satellites.debug);

    return elle::Status::Ok;
  }

  ///
  /// this method updates the in-memory parameters according to the
  /// associated settings.
  ///
  elle::Status          Configuration::Pull()
  {
    //
    // etoile
    //
    this->etoile.debug = (*this)["etoile"].Get("debug",
        Configuration::Default::Etoile::Debug);

    this->etoile.gear.containment = (*this)["etoile"].Get("gear.containment",
        Configuration::Default::Etoile::Gear::Containment);

    this->etoile.shrub.status = (*this)["etoile"].Get("shrub.status",
        Configuration::Default::Etoile::Shrub::Status);

    this->etoile.shrub.capacity = (*this)["etoile"].Get("shrub.capacity",
        Configuration::Default::Etoile::Shrub::Capacity);

    this->etoile.shrub.lifespan = (*this)["etoile"].Get("shrub.lifespan",
        Configuration::Default::Etoile::Shrub::Lifespan);

    this->etoile.shrub.frequency = (*this)["etoile"].Get("shrub.frequency",
        Configuration::Default::Etoile::Shrub::Frequency);

    this->etoile.history.status = (*this)["etoile"].Get("history.status",
        Configuration::Default::Etoile::History::Status);

    this->etoile.history.indicator.root =
      (*this)["etoile"].Get("history.indicator.root",
          Configuration::Default::Etoile::History::Indicator::Root);

    this->etoile.history.indicator.slab =
      (*this)["etoile"].Get("history.indicator.slab",
          Configuration::Default::Etoile::History::Indicator::Slab);

    //
    // nucleus
    //
    this->nucleus.debug = (*this)["nucleus"].Get("debug",
        Configuration::Default::Nucleus::Debug);

    //
    // hole
    //
    this->hole.debug = (*this)["hole"].Get("debug",
        Configuration::Default::Hole::Debug);

    //
    // horizon
    //
    this->horizon.debug = (*this)["horizon"].Get("debug",
        Configuration::Default::Horizon::Debug);

    //
    // satellites
    //
    this->satellites.debug = (*this)["satellites"].Get("debug",
        Configuration::Default::Satellites::Debug);

    return elle::Status::Ok;
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  //embed(Configuration, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a configuration.
  ///
  /// note that this method may actually not dump the current values of
  /// the parameters.
  ///
  elle::Status          Configuration::Dump(const elle::Natural32 margin) const
  {
    elle::String        alignment(margin, ' ');

    std::cout << alignment << "[Configuration]" << std::endl;

    // dump the parent settings.
    if (elle::Settings::Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the settings");

    return elle::Status::Ok;
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads the system's configuration file.
  ///
  elle::Status          Configuration::Load()
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Configuration) == elle::Status::Error)
      escape("unable to create the path");

    // call the setting's method.
    if (elle::Settings::Load(path) == elle::Status::Error)
      escape("unable to load the settings");

    return elle::Status::Ok;
  }

  ///
  /// this method stores the configuration.
  ///
  elle::Status          Configuration::Store() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Configuration) == elle::Status::Error)
      escape("unable to create the path");

    // call the setting's method.
    if (elle::Settings::Store(path) == elle::Status::Error)
      escape("unable to store the settings");

    return elle::Status::Ok;
  }

  ///
  /// this method erases the configuration.
  ///
  elle::Status          Configuration::Erase() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Configuration) == elle::Status::Error)
      escape("unable to create the path");

    // erase the file.
    if (elle::io::File::Erase(path) == elle::Status::Error)
      escape("unable to erase the file");

    return elle::Status::Ok;
  }

  ///
  /// this method tests the configuration.
  ///
  elle::Status          Configuration::Exist() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Configuration) == elle::Status::Error)
      escape("unable to create the path");

    // test the file.
    if (elle::io::File::Exist(path) == elle::Status::False)
      return elle::Status::False;

    return elle::Status::True;
  }

}
