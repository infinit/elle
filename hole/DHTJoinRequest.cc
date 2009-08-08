#include "hole/DHT.hh"
#include "hole/DHTRequest.hh"
#include "hole/DHTJoinRequest.hh"
#include "hole/protocol/Header.hh"

namespace hole
{
  DHTJoinRequest::DHTJoinRequest(DHT &                dht,
                                 const QHostAddress & address,
                                 quint16              port)
    : DHTRequest(dht),
      address_(address),
      port_(port)
  {
  }

  void
  DHTJoinRequest::Join()
  {
    QByteArray data;
    QDataStream stream(data);
    protocol::FindSuccessor cmd;

    cmd.header.id = protocol::Header::FindSuccessor;
    cmd.header.tag = tag_;
    cmd.header.length = cmd.Length();
    cmd.key = dht_.localNode_.key_;

    stream << cmd;
    dht_.socket_->writeDatagram(data, address_, port_);
  }

  void
  DHTJoinRequest::Received(const QByteArray & data)
  {
  }
}
