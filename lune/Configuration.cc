//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Configuration.cc
//
// created       julien quintard   [sun jun 19 23:19:22 2011]
// updated       julien quintard   [thu aug 11 11:53:11 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Configuration.hh>

#include <lune/Lune.hh>

#include <pig/PIG.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the configuration files extension.
  ///
  const elle::String	Configuration::Extension = ".conf";

  ///
  /// these are the default parameters.
  ///
  const elle::Boolean	Configuration::Default::Shrub::Status =
    true;
  const elle::Natural32	Configuration::Default::Shrub::Capacity =
    1024;
  const elle::Natural32	Configuration::Default::Shrub::Lifespan =
    300;
  const elle::Natural32	Configuration::Default::Shrub::Frequence =
    120000;

  const elle::Natural32	Configuration::Default::Cache::Capacity =
    2097152;

  const elle::Natural32	Configuration::Default::Reserve::Capacity =
    1073741824;

  const elle::Boolean	Configuration::Default::Debug::PIG =
    false;
  const elle::Boolean	Configuration::Default::Debug::Etoile =
    false;
  const elle::Boolean	Configuration::Default::Debug::Nucleus =
    false;
  const elle::Boolean	Configuration::Default::Debug::Hole =
    false;

  const elle::Boolean	Configuration::Default::History::Status =
    false;
  const elle::Character	Configuration::Default::History::Indicator::Root =
    '@';
  const elle::Character	Configuration::Default::History::Indicator::Slab =
    '%';

  const elle::String	Configuration::Default::FUSE::FUker =
    pig::FUker::TypeSequential;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method synchronises the in-memory configuration so as to
  /// be stored.
  ///
  elle::Status		Configuration::Push()
  {
    enter();

    //
    // update the settings with the parameters.
    //

    if (elle::Settings::Set(
	  "shrub", "status",
	  this->shrub.status) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "shrub", "capacity",
	  this->shrub.capacity) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "shrub", "lifespan",
	  this->shrub.lifespan) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "shrub", "frequence",
	  this->shrub.frequence) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "cache", "capacity",
	  this->cache.capacity) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "reserve", "capacity",
	  this->reserve.capacity) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "debug", "pig",
	  this->debug.pig) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "debug", "etoile",
	  this->debug.etoile) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "debug", "nucleus",
	  this->debug.nucleus) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "debug", "hole",
	  this->debug.hole) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "history", "status",
	  this->history.status) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "history", "indicator.root",
	  this->history.indicator.root) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "history", "indicator.slab",
	  this->history.indicator.slab) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "fuse", "fuker",
	  this->fuse.fuker) == elle::StatusError)
      escape("unable to update the parameter");

    leave();
  }

  ///
  /// this method updates the in-memory parameters according to the
  /// associated settings.
  ///
  elle::Status		Configuration::Pull()
  {
    enter();

    //
    // retrieve the parameters from the settings.
    //

    if (elle::Settings::Get(
	  "shrub", "status",
	  this->shrub.status,
	  Configuration::Default::Shrub::Status) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "shrub", "capacity",
	  this->shrub.capacity,
	  Configuration::Default::Shrub::Capacity) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "shrub", "lifespan",
	  this->shrub.lifespan,
	  Configuration::Default::Shrub::Lifespan) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "shrub", "frequence",
	  this->shrub.frequence,
	  Configuration::Default::Shrub::Frequence) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "cache", "capacity",
	  this->cache.capacity,
	  Configuration::Default::Cache::Capacity) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "reserve", "capacity",
	  this->reserve.capacity,
	  Configuration::Default::Reserve::Capacity) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "debug", "pig",
	  this->debug.pig,
	  Configuration::Default::Debug::PIG) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "debug", "etoile",
	  this->debug.etoile,
	  Configuration::Default::Debug::Etoile) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "debug", "nucleus",
	  this->debug.nucleus,
	  Configuration::Default::Debug::Nucleus) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "debug", "hole",
	  this->debug.hole,
	  Configuration::Default::Debug::Hole) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "history", "status",
	  this->history.status,
	  Configuration::Default::History::Status) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Get(
	  "history", "indicator.root",
	  this->history.indicator.root,
	  Configuration::Default::History::Indicator::Root) ==
	elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Get(
	  "history", "indicator.slab",
	  this->history.indicator.slab,
	  Configuration::Default::History::Indicator::Slab) ==
	elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Get(
	  "fuse", "fuker",
	  this->fuse.fuker,
	  Configuration::Default::FUSE::FUker) == elle::StatusError)
      escape("unable to update the parameter");

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
  elle::Status		Configuration::Dump(const elle::Natural32 margin) const
  {
    elle::String	alignment(margin, ' ');

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
  elle::Status		Configuration::Load()
  {
    elle::Path		path;

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
  elle::Status		Configuration::Store() const
  {
    elle::Path		path;

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
  elle::Status		Configuration::Erase() const
  {
    elle::Path		path;

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
  elle::Status		Configuration::Exist() const
  {
    elle::Path		path;

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
