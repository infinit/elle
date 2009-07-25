#include "hole/DHTJoinRequestHandler.hh"

namespace hole
{
  DHTJoinRequestHandler::DHTJoinRequestHandler(DHTJoinRequest & rq, QObject * parent)
    : QObject(parent),
      request_(rq),
      socket_(new QTcpSocket)
  {
  }

  void
  DHTJoinRequestHandler::Join()
  {
  }

  void
  DHTJoinRequestHandler::SocketConnected()
  {
  }

  void
  DHTJoinRequestHandler::SocketDisconnected()
  {
  }

  void
  DHTJoinRequestHandler::SocketError(QAbstractSocket::SocketError /*socketError*/)
  {
  }
}
