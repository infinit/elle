#include "hole/LocalNode.hh"

namespace hole
{
  LocalNode::LocalNode(DHT & dht)
    : Node(dht),
      server_(this)
  {
    connect(&server_, SIGNAL(newConnection()),
            this, SLOT(IncomingConnection()));
  }

  void
  LocalNode::IncomingConnection()
  {
  }
}
