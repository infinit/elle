//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphael Londeix   [Wed 29 Feb 2012 12:49:18 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <stdexcept>
#include <iostream>

#include <QJson/Parser>

#include "plasma/common/resources.hh"

#include "Connection.hh"
#include "LocalServer.hh"
#include "Manager.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

LocalServer::LocalServer(QApplication& app) :
  QLocalServer(&app),
  _state(State::Stopped),
  _manager(new Manager(app))
{
}

LocalServer::~LocalServer()
{
}

//
// ---------- methods  --------------------------------------------------------
//


void LocalServer::Start(std::string const& watchdogId)
{
  if (this->_state == State::Running)
    return;

  char const* id = watchdogId.c_str();
  // Trying to create a listening socket
  if (!this->listen(id))
    {
      std::cerr << "Warning: Server name already used (maybe previous crash)\n";

      // We try to remove the server instance first
      if (!QLocalServer::removeServer(id))
        throw std::runtime_error("Cannot remove previous server instance");

      // Then call listen again, since old socket have been removed
      if (!this->listen(id))
        throw std::runtime_error(
            "Cannot start the local server: " + watchdogId
        );
    }

  this->_state = State::Running;

  this->connect(
      this, SIGNAL(newConnection()),
      this, SLOT(_OnNewConnection())
  );
}

void LocalServer::_OnNewConnection()
{
  QLocalSocket* newSocket = nullptr;

  while ((newSocket = this->nextPendingConnection()) != nullptr)
  {
    std::cout << "New socket: " << newSocket << std::endl;
    ConnectionPtr conn{
      new Connection{
        std::unique_ptr<QLocalSocket>{newSocket}
      }
    };
    std::cout << "New connection: " << conn.get() << std::endl;
    this->_HandleNewConnection(conn);
  }
}

void LocalServer::_HandleNewConnection(ConnectionPtr& conn)
{
  std::cerr << "Handling\n";
  using namespace std::placeholders;
  conn->Connect(
      std::bind(&LocalServer::_OnClientCommand, this, conn, _1),
      std::bind(&LocalServer::_OnClientError, this, conn, _1)
  );
  // at last, saving it
  this->_manager->RegisterConnection(conn);
}

void LocalServer::_OnClientError(ConnectionPtr conn, std::string const& error)
{
  std::cerr << "Client has an error:" + error + "\n";
  this->_manager->UnregisterConnection(conn);
}

void LocalServer::_OnClientCommand(ConnectionPtr conn, QByteArray const& data)
{
  QJson::Parser parser;
  bool result;
  QVariantMap cmd = parser.parse(data, &result).toMap();
  if (!result)
    std::cerr << "Warning: Got invalid command: " << QString(data).toStdString();
  else
    this->_manager->ExecuteCommand(conn, cmd);
}
