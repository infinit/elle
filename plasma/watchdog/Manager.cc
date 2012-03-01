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
#include "Connection.hh"
#include "Manager.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

Manager::Manager(QApplication& app) :
  _app(app),
  _clients(new ClientMap)
{}

Manager::~Manager()
{
  delete this->_clients;
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

void Manager::ExecuteCommand(ConnectionPtr& conn, QVariantMap const& cmd)
{

}
