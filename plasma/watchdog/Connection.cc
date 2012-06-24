#include <cassert>

#include <elle/log.hh>

#include "Connection.hh"

using namespace plasma::watchdog;

Connection::Connection(QLocalSocketPtr&& socket) :
  QObject(),
  _socket(std::move(socket))
{
  this->_socket->setParent(this);
}

Connection::~Connection()
{}

void Connection::connect(Cmdback cmdback, Errback errback)
{
  elle::log::debug("Connecting !");
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
  elle::log::debug("Connected !");
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
      elle::log::debug("Got client command:", QString(line).toStdString());
      this->_cmdback(line);
    }
}
