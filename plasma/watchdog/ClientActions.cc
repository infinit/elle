//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix   [Thu 01 Mar 2012 12:55:56 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Client.hh"
#include "ClientActions.hh"
#include "Manager.hh"
#include "Connection.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

ClientActions::ClientActions(Manager& manager) :
  _manager(manager)
{
  using namespace std::placeholders;
  this->_manager.RegisterCommand(
      "RUN", std::bind(&ClientActions::_OnRun, this, _1, _2, _3)
  );
}

//
// ---------- methods  --------------------------------------------------------
//


void ClientActions::_OnRun(Connection& conn, Client& client, QVariantList const& args)
{}
