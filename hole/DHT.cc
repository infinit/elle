#include "hole/DHT.hh"

namespace hole
{
  DHT::DHT(QObject * p)
    : QObject(p),
      localNode(),
      nodes()
  {
  }

  DHT::~DHT()
  {
    foreach (Node * node, nodes)
      delete node;
  }
}
