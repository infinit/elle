#include "hole/DHTRequest.hh"
#include "hole/DHTJoinRequestHandler.hh"

namespace hole
{
  DHTJoinRequestHandler::DHTJoinRequestHandler(DHTJoinRequest & rq, QObject * p)
    : DHTRequestHandler(p),
      request_(rq)
  {
  }

  void
  DHTJoinRequestHandler::Join()
  {
    // Send the command
  }
}
