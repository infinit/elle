#include "hole/DHT.hh"

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
}
