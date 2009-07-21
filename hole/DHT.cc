#include "hole/DHT.hh"

namespace hole
{
  DHT::DHT(QObject * parent)
    : QObject(parent),
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
