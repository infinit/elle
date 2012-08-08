#include <stdexcept>
#include <iostream>

#include <QJson/Parser>

#include <elle/log.hh>

#include <common/common.hh>

#include "Connection.hh"
#include "LocalServer.hh"
#include "Manager.hh"

ELLE_LOG_COMPONENT("infinit.plasma.watchdog");

using namespace plasma::watchdog;

LocalServer::LocalServer(QCoreApplication& app) :
  QLocalServer(),
  _state(State::Stopped),
  _manager(new Manager(app))
{}

LocalServer::~LocalServer()
{}

void LocalServer::start(std::string const& watchdogId)
{
  if (this->_state == State::Running)
    return;

  this->_manager->start(watchdogId);

  // Trying to create a listening socket
  if (!this->listen(common::watchdog::server_name().c_str()))
    {
      ELLE_WARN("Server name already used (maybe previous crash)");

      // We try to remove the server instance first
      if (!QLocalServer::removeServer(common::watchdog::server_name().c_str()))
        throw std::runtime_error{"Cannot remove previous server instance"};

      // Then call listen again, since old socket have been removed
      if (!this->listen(common::watchdog::server_name().c_str()))
        throw std::runtime_error{
            "Cannot start the local server: " + common::watchdog::server_name()
        };
    }

  this->_state = State::Running;

  this->connect(
      this, SIGNAL(newConnection()),
      this, SLOT(_on_new_connection())
  );
}

void LocalServer::_on_new_connection()
{
  QLocalSocket* new_socket = nullptr;

  while ((new_socket = this->nextPendingConnection()) != nullptr)
  {
    ELLE_DEBUG("New socket: %s", new_socket);
    ConnectionPtr conn{
      new Connection{
        std::unique_ptr<QLocalSocket>{new_socket}
      }
    };
    ELLE_DEBUG("New connection: %s", conn.get());
    this->_handle_new_connection(conn);
  }
}

void LocalServer::_handle_new_connection(ConnectionPtr& conn)
{
  using namespace std::placeholders;
  conn->connect(
      std::bind(&LocalServer::_on_client_command, this, conn, _1),
      std::bind(&LocalServer::_on_client_error, this, conn, _1)
  );
  // at last, saving it
  this->_manager->register_connection(conn);
}

void LocalServer::_on_client_error(ConnectionPtr conn, std::string const& error)
{
  this->_manager->unregister_connection(conn);
}

void LocalServer::_on_client_command(ConnectionPtr conn, QByteArray const& data)
{
  QJson::Parser parser;
  bool result;
  QVariantMap cmd = parser.parse(data, &result).toMap();
  if (!result)
    ELLE_WARN("Got invalid command: %s", QString(data).toStdString());
  else if (!cmd.contains("_id"))
    ELLE_WARN("The command has to contain an _id.");
  else
    this->_manager->execute_command(conn, cmd);
}
