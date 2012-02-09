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

#if defined(INFINIT_LINUX)
  const elle::Natural32
  Configuration::Default::Horizon::FUker =
    static_cast<elle::Natural32>(horizon::linux::FUker::TypeSequential);
#elif defined(INFINIT_MACOSX)
  const elle::Natural32
  Configuration::Default::Horizon::FUker =
    static_cast<elle::Natural32>(horizon::macosx::FUker::TypeSequential);
#elif defined(INFINIT_WINDOWS)
  // XXX
#else
# error "unsupported platform"
#endif

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
    enter();

    //
    // etoile
    //
    {
      if (elle::Settings::Set(
            "etoile", "debug",
            this->etoile.debug) == elle::StatusError)
        escape("unable to update the parameter");

      if (elle::Settings::Set(
            "etoile", "gear.containment",
            this->etoile.gear.containment) == elle::StatusError)
        escape("unable to update the parameter");

      if (elle::Settings::Set(
            "etoile", "shrub.status",
            this->etoile.shrub.status) == elle::StatusError)
        escape("unable to update the parameter");

      if (elle::Settings::Set(
            "etoile", "shrub.capacity",
            this->etoile.shrub.capacity) == elle::StatusError)
        escape("unable to update the parameter");

      if (elle::Settings::Set(
            "etoile", "shrub.lifespan",
            this->etoile.shrub.lifespan) == elle::StatusError)
        escape("unable to update the parameter");

      if (elle::Settings::Set(
            "etoile", "shrub.frequency",
            this->etoile.shrub.frequency) == elle::StatusError)
        escape("unable to update the parameter");

      if (elle::Settings::Set(
            "etoile", "history.status",
            this->etoile.history.status) == elle::StatusError)
      escape("unable to update the parameter");

      if (elle::Settings::Set(
            "etoile", "history.indicator.root",
            this->etoile.history.indicator.root) == elle::StatusError)
        escape("unable to update the parameter");

      if (elle::Settings::Set(
            "etoile", "history.indicator.slab",
            this->etoile.history.indicator.slab) == elle::StatusError)
        escape("unable to update the parameter");
    }

    //
    // nucleus
    //
    {
      if (elle::Settings::Set(
            "nucleus", "debug",
            this->nucleus.debug) == elle::StatusError)
        escape("unable to update the parameter");
    }

    //
    // hole
    //
    {
      if (elle::Settings::Set(
            "hole", "debug",
            this->hole.debug) == elle::StatusError)
        escape("unable to update the parameter");
    }

    //
    // horizon
    //
    {
      if (elle::Settings::Set(
            "horizon", "debug",
            this->horizon.debug) == elle::StatusError)
        escape("unable to update the parameter");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
      if (elle::Settings::Set(
            "horizon", "fuker",
            this->horizon.fuker) == elle::StatusError)
        escape("unable to update the parameter");
#elif defined(INFINIT_WINDOWS)
      // XXX
#else
# error "unsupported platform"
#endif
    }

    //
    // satellites
    //
    {
      if (elle::Settings::Set(
            "satellites", "debug",
            this->satellites.debug) == elle::StatusError)
        escape("unable to update the parameter");
    }

    leave();
  }

  ///
  /// this method updates the in-memory parameters according to the
  /// associated settings.
  ///
  elle::Status          Configuration::Pull()
  {
    enter();

    //
    // etoile
    //
    {
      if (elle::Settings::Get(
            "etoile", "debug",
            this->etoile.debug,
            Configuration::Default::Etoile::Debug) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");

      if (elle::Settings::Get(
            "etoile", "gear.containment",
            this->etoile.gear.containment,
            Configuration::Default::Etoile::Gear::Containment) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");

      if (elle::Settings::Get(
            "etoile", "shrub.status",
            this->etoile.shrub.status,
            Configuration::Default::Etoile::Shrub::Status) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");

      if (elle::Settings::Get(
            "etoile", "shrub.capacity",
            this->etoile.shrub.capacity,
            Configuration::Default::Etoile::Shrub::Capacity) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");

      if (elle::Settings::Get(
            "etoile", "shrub.lifespan",
            this->etoile.shrub.lifespan,
            Configuration::Default::Etoile::Shrub::Lifespan) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");

      if (elle::Settings::Get(
            "etoile", "shrub.frequency",
            this->etoile.shrub.frequency,
            Configuration::Default::Etoile::Shrub::Frequency) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");

      if (elle::Settings::Get(
            "etoile", "history.status",
            this->etoile.history.status,
            Configuration::Default::Etoile::History::Status) ==
          elle::StatusError)
        escape("unable to update the parameter");

      if (elle::Settings::Get(
            "etoile", "history.indicator.root",
            this->etoile.history.indicator.root,
            Configuration::Default::Etoile::History::Indicator::Root) ==
          elle::StatusError)
        escape("unable to update the parameter");

      if (elle::Settings::Get(
            "etoile", "history.indicator.slab",
            this->etoile.history.indicator.slab,
            Configuration::Default::Etoile::History::Indicator::Slab) ==
          elle::StatusError)
        escape("unable to update the parameter");
    }

    //
    // nucleus
    //
    {
      if (elle::Settings::Get(
            "nucleus", "debug",
            this->nucleus.debug,
            Configuration::Default::Nucleus::Debug) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");
    }

    //
    // hole
    //
    {
      if (elle::Settings::Get(
            "hole", "debug",
            this->hole.debug,
            Configuration::Default::Hole::Debug) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");
    }

    //
    // horizon
    //
    {
      if (elle::Settings::Get(
            "horizon", "debug",
            this->horizon.debug,
            Configuration::Default::Horizon::Debug) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
      if (elle::Settings::Get(
            "horizon", "fuker",
            this->horizon.fuker,
            Configuration::Default::Horizon::FUker) == elle::StatusError)
        escape("unable to update the parameter");
#elif defined(INFINIT_WINDOWS)
      // XXX
#else
# error "unsupported platform"
#endif
    }

    //
    // satellites
    //
    {
      if (elle::Settings::Get(
            "satellites", "debug",
            this->satellites.debug,
            Configuration::Default::Satellites::Debug) ==
          elle::StatusError)
        escape("unable to retrieve the parameter");
    }

    leave();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Configuration, _());

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

    enter();

    std::cout << alignment << "[Configuration]" << std::endl;

    // dump the parent settings.
    if (elle::Settings::Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the settings");

    leave();
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

    enter();

    // create the path.
    if (path.Create(Lune::Configuration) == elle::StatusError)
      escape("unable to create the path");

    // call the setting's method.
    if (elle::Settings::Load(path) == elle::StatusError)
      escape("unable to load the settings");

    leave();
  }

  ///
  /// this method stores the configuration.
  ///
  elle::Status          Configuration::Store() const
  {
    elle::Path          path;

    enter();

    // create the path.
    if (path.Create(Lune::Configuration) == elle::StatusError)
      escape("unable to create the path");

    // call the setting's method.
    if (elle::Settings::Store(path) == elle::StatusError)
      escape("unable to store the settings");

    leave();
  }

  ///
  /// this method erases the configuration.
  ///
  elle::Status          Configuration::Erase() const
  {
    elle::Path          path;

    enter();

    // create the path.
    if (path.Create(Lune::Configuration) == elle::StatusError)
      escape("unable to create the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    leave();
  }

  ///
  /// this method tests the configuration.
  ///
  elle::Status          Configuration::Exist() const
  {
    elle::Path          path;

    enter();

    // create the path.
    if (path.Create(Lune::Configuration) == elle::StatusError)
      escape("unable to create the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      false();

    true();
  }

}
