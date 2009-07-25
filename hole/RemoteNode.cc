#include <assert.h>

#include "hole/RemoteNode.hh"

namespace hole
{
  RemoteNode::RemoteNode(DHT & dht, QTcpSocket * socket)
    : Node(dht),
      socket_(socket)
  {
    assert(socket_);
  }

  QHostAddress
  RemoteNode::Address() const
  {
    return socket_->peerAddress();
  }

  quint16
      RemoteNode::Port() const
  {
    return socket_->peerPort();
  }

  void
  RemoteNode::Disconnect()
  {
    // send the disconnect & leave message
    // closes the socket when finished
    socket_->close();
  }
}
