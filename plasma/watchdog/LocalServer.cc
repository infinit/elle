#include <stdexcept>
#include <iostream>

#include <QJson/Parser>

#include <elle/log.hh>

#include <common/common.hh>

#include "Manager.hh"
#include "Connection.hh"
#include "LocalServer.hh"

ELLE_LOG_COMPONENT("infinit.plasma.watchdog.LocalServer");

using namespace plasma::watchdog;

LocalServer::LocalServer(QCoreApplication& app,
                         std::string const& user_id):
  QLocalServer(),
  _state(State::Stopped),
  _manager{new Manager{app, *this, user_id}},
  _user_id{user_id}
{
  ELLE_DEBUG("Creating local server for user %s", user_id);
}

void LocalServer::start(std::string const& watchdogId)
{
  if (this->_state == State::Running)
    return;

  this->_manager->start(watchdogId);

  std::string server_name = common::watchdog::server_name(_user_id);
  ELLE_DEBUG("Trying to listen on %s", server_name);
  // Trying to create a listening socket
  if (!this->listen(server_name.c_str()))
    {
      ELLE_WARN("Server name %s already used (maybe previous crash) and"
                " will be removed.", server_name);

      // We try to remove the server instance first
      if (!QLocalServer::removeServer(server_name.c_str()))
        throw std::runtime_error{"Cannot remove previous server instance"};

      // Then call listen again, since old socket have been removed
      if (!this->listen(server_name.c_str()))
        throw std::runtime_error{
            "Cannot start the local server: " + server_name
        };
    }

  this->_state = State::Running;

  this->connect(
      this, SIGNAL(newConnection()),
      this, SLOT(_on_new_connection())
  );
}

void LocalServer::stop()
{
  ELLE_TRACE("Closing local server");
  this->close();
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
  conn->connect(
    std::bind(&LocalServer::_on_client_command, this, conn, std::placeholders::_1),
    std::bind(&LocalServer::_on_client_error, this, conn, std::placeholders::_1)
  );
  // at last, saving it
  this->_manager->register_connection(conn);
}

void LocalServer::_on_client_error(ConnectionPtr conn, std::string const&)
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
