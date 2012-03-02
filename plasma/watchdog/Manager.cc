//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix [Thu 01 Mar 2012 11:16:34 AM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <cassert>
#include <stdexcept>
#include <iostream>

#include "Client.hh"
#include "ClientActions.hh"
#include "Connection.hh"
#include "Manager.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

Manager::Manager(QApplication& app) :
  _app(app),
  _clients(new ClientMap()),
  _commands(new CommandMap()),
  _actions(new ClientActions(*this))
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

//
// ---------- methods  --------------------------------------------------------
//

Client& Manager::RegisterConnection(ConnectionPtr& conn)
{
  assert(conn != nullptr);
  auto it = this->_clients->find(conn);
  if (it != this->_clients->end())
    {
      std::cerr << "Warning: The connection is already registered.\n";
      return *(it->second);
    }
  auto res = this->_clients->insert(
      ClientMap::value_type(conn, ClientPtr(new Client))
  );
  if (res.second == false)
    throw std::runtime_error("Couldn't insert the new client");
  return *(res.first->second);
}

void Manager::UnregisterConnection(ConnectionPtr& conn)
{
  auto it = this->_clients->find(conn);
  if (it == this->_clients->end())
      std::cerr << "Warning: The connection is not registered.\n";
  else
    this->_clients->erase(it);
}

void Manager::RegisterCommand(std::string const& id, Command cmd)
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

void Manager::UnregisterCommand(std::string const& id)
{
  auto it = this->_commands->find(id);
  if (it == this->_commands->end())
    throw std::runtime_error(
        "The command '" + id + "' is not registered !"
    );
  this->_commands->erase(it);
}

void Manager::UnregisterAllCommands()
{
  this->_commands->clear();
}


void Manager::ExecuteCommand(ConnectionPtr& conn, QVariantMap const& cmd)
{
  auto it = this->_commands->find(cmd["command"].toString().toStdString());
  if (it == this->_commands->end())
    {
      std::cerr << "Warning: command not found: "
                << cmd["command"].toString().toStdString() << ".\n";
      return;
    }
  (it->second)(*conn, *(*this->_clients)[conn], cmd["arguments"].toList());
}

void Manager::Stop()
{
}

void Manager::Start(std::string const& watchdogId)
{
  this->_actions->watchdogId(watchdogId);
}
