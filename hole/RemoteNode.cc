#include "hole/RemoteNode.hh"

namespace hole
{
  QHostAddress
  RemoteNode::Address() const
  {
    return socket_.peerAddress();
  }

  quint16
      RemoteNode::Port() const
  {
    return socket_.peerPort();
  }

  void
  RemoteNode::Disconnect()
  {
    // send the disconnect & leave message
    // closes the socket when finished
  }
}
