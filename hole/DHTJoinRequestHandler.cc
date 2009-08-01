#include "hole/DHT.hh"
#include "hole/DHTRequest.hh"
#include "hole/DHTJoinRequestHandler.hh"
#include "hole/protocol/Header.hh"

namespace hole
{
  DHTJoinRequestHandler::DHTJoinRequestHandler(DHT & dht, DHTJoinRequest & rq)
    : DHTRequestHandler(dht),
      request_(rq)
  {
  }

  void
  DHTJoinRequestHandler::Join()
  {
    QByteArray data;
    QDataStream stream(data);
    protocol::FindSuccessor cmd;

    cmd.header.id = protocol::Header::FindSuccessor;
    cmd.header.tag = tag_;
    cmd.header.length = cmd.Length();
    cmd.key = dht_.localNode_.key_;

    stream << cmd;
    dht_.socket_->writeDatagram(data, request_.address, request_.port);
  }
}
