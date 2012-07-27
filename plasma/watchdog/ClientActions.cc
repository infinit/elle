#include <fstream>

#include <elle/format/json.hh>
#include <elle/log.hh>
#include <elle/os/path.hh>

#include <common/common.hh>

#include "Client.hh"
#include "ClientActions.hh"
#include "Connection.hh"
#include "InfinitNetwork.hh"
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
      REGISTER(status);                                                       \
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
  QString user = args["user"].toString();
  if (token.size() > 0 && identity.size() > 0)
    {
      this->_manager.token(token);
      this->_manager.identity(identity);
      this->_manager.user(user);

      std::ofstream identity_infos{common::watchdog::identity_path()};

      if (!identity_infos.good())
        {
          elle::log::fatal("Cannot open identity file");
          std::abort();
        }

      identity_infos << token.toStdString() << "\n"
                     << identity.toStdString() << "\n"
                     << user.toStdString() << "\n"
                     ;

      if (!identity_infos.good())
        {
          elle::log::fatal("Cannot write identity file");
          std::abort();
        }
      identity_infos.close();

      this->_manager.start_refresh_networks();
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


void ClientActions::_on_status(Connection& conn,
                               Client& client,
                               QVariantMap const& args)
{
  namespace json = elle::format::json;

  CHECK_ID(args);

  json::Array networks;
  for (auto& pair : this->_manager.network_manager().networks())
    {
      json::Dictionary network;
      network["_id"] = pair.first;
      network["mount_point"] = pair.second->mount_point();
      network["user"] = this->_manager.user();
      networks.push_back(network);
    }

  json::Dictionary response;
  response["networks"] = networks;
  conn.send_data((response.repr() + "\n").c_str());
}
