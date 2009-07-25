#include "hole/DHTJoinRequestHandler.hh"

namespace hole
{
  DHTJoinRequestHandler::DHTJoinRequestHandler(DHTJoinRequest & rq, QObject * parent)
    : QObject(parent),
      request_(rq)
  {
  }

  void
  DHTJoinRequestHandler::connected()
  {
  }

  void
  DHTJoinRequestHandler::disconnected()
  {
  }

  void
  DHTJoinRequestHandler::error(QAbstractSocket::SocketError /*socketError*/)
  {
  }
}
