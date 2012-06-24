#include <iostream>

#include <elle/log.hh>

#include "Client.hh"
#include "ClientActions.hh"
#include "Connection.hh"
#include "Manager.hh"
#include "NetworkManager.hh"

using namespace plasma::watchdog;


//
// ---------- helper macros ---------------------------------------------------
//

///
/// Register a command to the manager.
///
#define REGISTER(name, method)                                                \
  do {                                                                        \
    using namespace std::placeholders;                                        \
    this->_manager.RegisterCommand(                                           \
        name,                                                                 \
        std::bind(&ClientActions::method, this, _1, _2, _3)                   \
    );                                                                        \
  } while(false)                                                              \

#define UNREGISTER(name)                                                      \
  this->_manager.UnregisterCommand(name)                                      \

///
/// Security check (the watchdog id is valid)
///
#define CHECK_ID(args)                                                        \
  do {                                                                        \
      if (args["_id"].toString() != this->_watchdogId)                        \
        {                                                                     \
          std::cerr << "Warning: Invalid watchdog id: "                       \
                    << args["_id"].toString().toStdString() << "\n";          \
          return;                                                             \
        }                                                                     \
  } while(false)                                                              \

ClientActions::ClientActions(Manager& manager) :
  _manager(manager)
{
  REGISTER("run", _OnRun);
  REGISTER("stop", _OnStop);
}

ClientActions::~ClientActions()
{}

void ClientActions::_OnRun(Connection& conn,
                           Client& client,
                           QVariantMap const& args)
{
  elle::log::debug("Starting watchdog monitoring.");
  CHECK_ID(args);
  QString token = args["token"].toString();
  QString identity = args["identity"].toString();
  if (token.size() > 0 && identity.size() > 0)
    {
      this->_manager.token(token);
      this->_manager.identity(identity);
      this->_manager.networkManager().UpdateNetworks();
      UNREGISTER("run");
    }
  else
    elle::log::warn("The token was not provided (cannot start monitoring).");
}

void ClientActions::_OnStop(Connection& conn,
                           Client& client,
                           QVariantMap const& args)
{
  this->_manager.Stop();
}
