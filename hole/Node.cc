#include "hole/Node.hh"

namespace hole
{
  Node::Node(DHT & dht)
    : dht_(dht),
      key_()
  {
  }

  Node::~Node()
  {
  }
}
