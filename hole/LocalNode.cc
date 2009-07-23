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

  void
  LocalNode::Listen(quint16 port)
  {
    server_.listen(QHostAddress::Any, port);
  }

  quint16
  LocalNode::Port() const
  {
    return server_.serverPort();
  }

  QHostAddress
  LocalNode::Address() const
  {
    return server_.serverAddress();
  }

  void
  LocalNode::Disconnect()
  {
    server_.close();
  }
}
