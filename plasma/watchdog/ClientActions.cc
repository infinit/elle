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
      REGISTER(file_infos);                                                   \
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

      std::ofstream identity_infos{
          elle::os::path::join(common::infinit_home(), "identity.wtg")
      };

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


void ClientActions::_on_file_infos(Connection& conn,
                                   Client& client,
                                   QVariantMap const& args)
{
  CHECK_ID(args);
  std::string abspath = args["absolute_path"].toString().toStdString();
  InfinitNetwork* network = nullptr;
  std::string relpath;
  for (auto& pair : this->_manager.network_manager().networks())
    {
      std::string mount_point = pair.second->mount_point();
      // XXX should rewritten using boost::filesystem
      if (abspath.substr(0, mount_point.size()) == mount_point)
        {
          network = pair.second.get();
          relpath = "./" + abspath.substr(mount_point.size());
          break;
        }
    }

  if (network == nullptr)
    {
      elle::log::error("Cannot find network for '" + abspath + "' path");
      return;
    }

  elle::format::json::Dictionary response;
  response["network_id"] = network->id();
  response["mount_point"] = network->mount_point();
  response["absolute_path"] = abspath;
  response["relative_path"] = relpath;
  conn.send_data((response.repr() + "\n").c_str());
}
