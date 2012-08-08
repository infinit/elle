#include <cassert>
#include <functional>
#include <iostream>
#include <stdexcept>

#include <elle/log.hh>
#include <common/common.hh>

#include "Client.hh"
#include "ClientActions.hh"
#include "Connection.hh"
#include "Manager.hh"
#include "NetworkManager.hh"

ELLE_LOG_COMPONENT("infinit.plasma.watchdog");

using namespace plasma::watchdog;

Manager::Manager(QCoreApplication& app) :
  _app(app),
  _clients(new ClientMap()),
  _commands(new CommandMap()),
  _actions(new ClientActions(*this)),
  _network_manager(new NetworkManager(*this)),
  _meta(common::meta::host(), common::meta::port())
{}

Manager::~Manager()
{
  delete this->_clients;
  this->_clients = nullptr;
  delete this->_commands;
  this->_commands = nullptr;
  delete this->_actions;
  this->_actions = nullptr;
}

void Manager::token(QByteArray const& token)
{
  this->_meta.token(token.constData());
}

Client& Manager::register_connection(ConnectionPtr& conn)
{
  assert(conn != nullptr);
  auto it = this->_clients->find(conn);
  if (it != this->_clients->end())
    {
      ELLE_WARN("the connection %s is already registered", conn.get());
      return *(it->second);
    }
  auto res = this->_clients->insert(
      ClientMap::value_type(conn, ClientPtr(new Client(this->_meta)))
  );
  if (res.second == false)
    throw std::runtime_error("Couldn't insert the new client");
  return *(res.first->second);
}

void Manager::unregister_connection(ConnectionPtr& conn)
{
  auto it = this->_clients->find(conn);
  if (it == this->_clients->end())
    ELLE_WARN("the connection %s is not registered", conn.get());
  else
    this->_clients->erase(it);
}

void Manager::register_command(std::string const& id, Command cmd)
{
  if (this->_commands->find(id) != this->_commands->end())
    throw std::runtime_error(
        "The command '" + id + "' is already registered !"
    );
  auto res = this->_commands->insert(
      CommandMap::value_type(id, cmd)
  );
  if (res.second == false)
    throw std::runtime_error("Could not register a new command");
}

void Manager::unregister_command(std::string const& id)
{
  auto it = this->_commands->find(id);
  if (it == this->_commands->end())
    throw std::runtime_error(
        "The command '" + id + "' is not registered !"
    );
  this->_commands->erase(it);
}

void Manager::unregister_all_commands()
{
  this->_commands->clear();
}


void Manager::execute_command(ConnectionPtr& conn, QVariantMap const& cmd)
{
  if (cmd["_id"].toString() != this->_actions->watchdogId())
    {
      ELLE_WARN("Invalid given watchdog id: %s",
                      cmd["_id"].toString().toStdString());
      return;
    }

  auto it = this->_commands->find(cmd["command"].toString().toStdString());
  if (it == this->_commands->end())
    {
      ELLE_WARN("command not found: %s",
                      cmd["command"].toString().toStdString());
      return;
    }

  (it->second)(*conn, *(*this->_clients)[conn], cmd);
}

void Manager::stop()
{
  ELLE_DEBUG("Shutting down the watchdog");
  this->_network_manager->stop();
  this->_app.quit();
}

void Manager::start(std::string const& watchdogId)
{
  this->_actions->watchdogId(watchdogId.c_str());
}

void Manager::refresh_networks()
{
  this->_network_manager->update_networks();
}

void Manager::start_refresh_networks()
{
  this->refresh_networks();
  if (this->_timer.isActive())
    {
      ELLE_WARN("the timer is already activated");
      return;
    }
  this->connect(&this->_timer,
                SIGNAL(timeout()),
                this,
                SLOT(_on_timeout()));
  this->_timer.start(30000); // 30 sec
}

void Manager::_on_timeout()
{
  this->refresh_networks();
}
