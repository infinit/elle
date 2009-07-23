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
  DHT::Disconnect()
  {
    // Close local socket
    // Disconnect from every peers
  }
}
