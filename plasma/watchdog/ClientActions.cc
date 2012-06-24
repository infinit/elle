#include <iostream>

#include <elle/log.hh>

#include "Client.hh"
#include "ClientActions.hh"
#include "Connection.hh"
#include "Manager.hh"
#include "NetworkManager.hh"

using namespace plasma::watchdog;

// Register a command to the manager.
#define REGISTER(name)                                                        \
  do {                                                                        \
    using namespace std::placeholders;                                        \
    this->_manager.register_command(                                          \
        #name,                                                                \
        std::bind(&ClientActions::_on_##name, this, _1, _2, _3)               \
    );                                                                        \
  } while(false)                                                              \
  /**/

#define UNREGISTER(name)                                                      \
  this->_manager.unregister_command(#name)                                    \
  /**/

// Security check (the watchdog id is valid)
#define CHECK_ID(args)                                                        \
  do {                                                                        \
      if (args["_id"].toString() != this->_watchdogId)                        \
        {                                                                     \
          elle::log::warn("Invalid watchdog id:",                             \
                          args["_id"].toString().toStdString());              \
          return;                                                             \
        }                                                                     \
  } while(false)                                                              \
  /**/

// Register all commands except "run" and "stop"
# define REGISTER_ALL()                                                       \
  do {                                                                        \
      REGISTER(refresh_networks);                                             \
  } while (false)                                                             \
  /**/

ClientActions::ClientActions(Manager& manager) :
  _manager(manager)
{
  REGISTER(run);
  REGISTER(stop);
}

ClientActions::~ClientActions()
{}

void ClientActions::_on_run(Connection& conn,
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
      this->_manager.refresh_networks();
      UNREGISTER(run);
      REGISTER_ALL();
    }
  else
    elle::log::warn("The token was not provided (cannot start monitoring).");
}

void ClientActions::_on_stop(Connection& conn,
                            Client& client,
                            QVariantMap const& args)
{
  CHECK_ID(args);
  this->_manager.unregister_all_commands();
  this->_manager.stop();
}


void ClientActions::_on_refresh_networks(Connection& conn,
                                         Client& client,
                                         QVariantMap const& args)
{
  CHECK_ID(args);
  this->_manager.refresh_networks();
}
