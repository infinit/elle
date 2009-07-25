#include <assert.h>

#include "hole/DHT.hh"
#include "hole/DHTJoinRequestHandler.hh"

namespace hole
{
  DHT::DHT(QObject * p)
    : QObject(p),
      localNode_(*this),
      nodes_()
  {
  }

  DHT::~DHT()
  {
    foreach (Node * node, nodes_)
      delete node;
  }

  void
  DHT::Port(quint16 port)
  {
    localNode_.Listen(port);
  }

  quint16
  DHT::Port() const
  {
    return localNode_.Port();
  }

  void
  DHT::Create()
  {
    // TODO
  }

  void
  DHT::Join(DHTJoinRequest * request)
  {
    assert(request);
    DHTJoinRequestHandler * handler = new DHTJoinRequestHandler(*request);
    handler->Join();
    connect(handler, SIGNAL(Joined(DHTJoinRequestHandler *)),
            this, SLOT(Joined(DHTJoinRequestHandler *)));
  }

  void
  DHT::Joined(DHTJoinRequestHandler * handler)
  {
  }

  void
  DHT::Disconnect()
  {
    // Close local socket
    // Disconnect from every peers
  }
}
