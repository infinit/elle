//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix   [Thu 01 Mar 2012 01:52:17 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Agent.hh"
#include "WatchdogInterface.hh"

using namespace plasma::watchdog;

//
// ---------- attributes ------------------------------------------------------
//

//
// ---------- contructors & descructors ---------------------------------------
//

Agent::Agent(std::unique_ptr<WatchdogInterface>&& interface) :
  _interface(std::move(interface))
{}

//
// ---------- methods  --------------------------------------------------------
//


void Agent::Run()
{}


