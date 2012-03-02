//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphael Londeix   [Wed 29 Feb 2012 02:15:16 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <iostream>
#include <cassert>

#include "Connection.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

Connection::Connection(QLocalSocketPtr&& socket) :
  QObject(),
  _socket(std::move(socket))
{
  this->_socket->setParent(this);
}

Connection::~Connection()
{
  std::cerr << "Connection::~Connection()\n";
}

//
// ---------- methods  --------------------------------------------------------
//

void Connection::Connect(Cmdback cmdback, Errback errback)
{
  std::cerr << "Connecting !\n";
  this->_cmdback = cmdback;
  this->_errback = errback;
  assert(this->_cmdback != nullptr && this->_errback != nullptr);
  assert(this->_socket != nullptr);
  this->connect(
      this->_socket.get(), SIGNAL(disconnected()),
      this, SLOT(_OnDisconnect())
  );
  this->connect(
      this->_socket.get(), SIGNAL(readyRead()),
      this, SLOT(_OnReadyRead())
  );
  std::cerr << "Connected !\n";
}

void Connection::_Raise(std::string const& error)
{
  if (this->_errback != nullptr)
    this->_errback(error);
}

void Connection::_OnDisconnect()
{
  this->_Raise("Client disconnected");
}


void Connection::_OnReadyRead()
{
  while (this->_socket->canReadLine())
    {
      QByteArray line = this->_socket->readLine();
      std::cout << "Got client command: " << QString(line).toStdString();
      this->_cmdback(line);
    }
}
