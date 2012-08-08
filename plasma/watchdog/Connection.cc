#include <cassert>

#include <elle/log.hh>

#include "Connection.hh"

ELLE_LOG_COMPONENT("infinit.plasma.watchdog");

using namespace plasma::watchdog;

Connection::Connection(QLocalSocketPtr&& socket) :
  QObject(),
  _socket(std::move(socket))
{
  //this->_socket->setParent(this);
}

Connection::~Connection()
{}

void Connection::connect(Cmdback cmdback, Errback errback)
{
  ELLE_DEBUG("Connecting !");
  this->_cmdback = cmdback;
  this->_errback = errback;
  assert(this->_cmdback != nullptr && this->_errback != nullptr);
  assert(this->_socket != nullptr);
  this->connect(
      this->_socket.get(), SIGNAL(disconnected()),
      this, SLOT(_on_disconnect())
  );
  this->connect(
      this->_socket.get(), SIGNAL(readyRead()),
      this, SLOT(_on_ready_read())
  );
  ELLE_DEBUG("Connected !");
}

void Connection::_raise(std::string const& error)
{
  if (this->_errback != nullptr)
    this->_errback(error);
}

void Connection::_on_disconnect()
{
  this->_raise("Client disconnected");
}


void Connection::_on_ready_read()
{
  while (this->_socket->canReadLine())
    {
      QByteArray line = this->_socket->readLine();
      ELLE_DEBUG("Got client command: %s", QString(line).toStdString());
      this->_cmdback(line);
    }
}

void Connection::send_data(QByteArray const& data)
{
  // XXX async needed here
  if (this->_socket->write(data) != data.size())
    throw std::runtime_error("Not written !");
}
