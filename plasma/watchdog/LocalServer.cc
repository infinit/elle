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
  QLocalServer(),
  _state(State::Stopped),
  _manager(new Manager(app))
{
}

LocalServer::~LocalServer()
{
  std::cerr << "LocalServer::~LocalServer()\n";
}

//
// ---------- methods  --------------------------------------------------------
//


void LocalServer::Start(std::string const& watchdogId)
{
  if (this->_state == State::Running)
    return;

  this->_manager->Start(watchdogId);

  // Trying to create a listening socket
  if (!this->listen(WATCHDOG_SERVER_NAME))
    {
      std::cerr << "Warning: Server name already used (maybe previous crash)\n";

      // We try to remove the server instance first
      if (!QLocalServer::removeServer(WATCHDOG_SERVER_NAME))
        throw std::runtime_error("Cannot remove previous server instance");

      // Then call listen again, since old socket have been removed
      if (!this->listen(WATCHDOG_SERVER_NAME))
        throw std::runtime_error(
            "Cannot start the local server: " WATCHDOG_SERVER_NAME
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
  else if (!cmd.contains("_id"))
    std::cerr << "Warning: The command has to contain an _id.\n";
  else
    this->_manager->ExecuteCommand(conn, cmd);
}
