#include "hole/DHTRequest.hh"

namespace hole
{
  /* DHTRequest stuff */
  DHTRequest::~DHTRequest()
  {
  }

  void
  DHTRequest::done()
  {
  }

  void
  DHTRequest::failed()
  {
  }


  /* DHTJoinRequest stuff */
  void
  DHTJoinRequest::connected()
  {
  }

  void
  DHTJoinRequest::disconnected()
  {
  }

  void
  DHTJoinRequest::error(QAbstractSocket::SocketError /*socketError*/)
  {
  }
}
