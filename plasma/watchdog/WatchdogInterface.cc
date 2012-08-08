//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix   [Thu 01 Mar 2012 01:49:03 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Application.hh"
#include "WatchdogInterface.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

WatchdogInterface::WatchdogInterface(Application& app) :
  _app(app)
{
}

//
// ---------- methods  --------------------------------------------------------
//

std::vector<WatchdogInterface::Directory> WatchdogInterface::DirListing(std::string const&)
{
  return std::vector<Directory>();
}

