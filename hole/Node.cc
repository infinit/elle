#include "hole/Node.hh"

namespace hole
{
  Node::Node(DHT & dht)
    : dht_(dht),
      key_(),
      address_(),
      port_()
  {
  }
}
