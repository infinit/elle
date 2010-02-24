#include <assert.h>
#include <time.h>
#include <QByteArray>
#include <QDataStream>

#include "hole/DHT.hh"
#include "hole/DHTRequest.hh"
#include "hole/DHTJoinRequest.hh"

namespace hole
{
  DHT::DHT(QObject * p)
    : QObject(p),
      localNode_(),
      fingerTable_(localNode_),
      socket_(new QUdpSocket(this)),
      port_(64626),
      requests_(),
      joinRequests_()
  {
    connect(socket_, SIGNAL(readyRead()), this, SLOT(ReadDatagram()));
  }

  DHT::~DHT()
  {
  }

  void
  DHT::Port(quint16 port)
  {
    if (port == port_)
      return;

    port_ = port;
    if (socket_->state() == QAbstractSocket::BoundState)
      socket_->bind(port_);
  }

  quint16
  DHT::Port() const
  {
    return port_;
  }

  void
  DHT::ReadDatagram()
  {
    FullTag    fullTag;
    QByteArray data;

    data.resize(socket_->pendingDatagramSize());
    if (socket_->readDatagram(data.data(), data.size(),
                              &fullTag.address, &fullTag.port) <= 0)
      return;

    // decode data
    QDataStream stream(data);
    protocol::CmdId cmd_id;
    stream >> cmd_id >> fullTag.tag;
  }

  void
  DHT::HandleCommand(const QByteArray & data,
                     protocol::CmdId    cmdId,
                     const FullTag &    fullTag)
  {
    switch (cmdId)
    {
    case protocol::Header::Ping:
      SendPong(fullTag);
      return;
    case protocol::Header::Quit:
      return;
    case protocol::Header::FindSuccessor:
      return;
    case protocol::Header::FindSuccessorBounce:
      return;
    case protocol::Header::FindSuccessorFound:
      JoinRequests_t::Iterator it = joinRequests_.find(fullTag.tag);
      if (it != joinRequests_.end())
        it.value()->Received(data);
      return;
    }
  }

  void
  DHT::HandleFindSuccessor(const FullTag & fullTag,
                           const QByteArray & data)
  {
    protocol::FindSuccessor rq;
    QDataStream stream(data);

    stream >> rq;
    if (stream.status() != QDataStream::Ok)
      return; // corrupted data

    if (localNode_.key < rq.key && rq.key <= fingerTable_.Successor().key)
      ;
  }

  DHTRequest *
  DHT::Ping(Node * node)
  {
    DHTRequest * request = new DHTRequest(*this);
    QByteArray data;
    QDataStream stream(data);
    protocol::Header packet;

    request->tag_ = 0;  // No need for a special tag
    packet.id = protocol::Header::Ping;
    packet.tag = request->tag_;
    packet.length = 0;
    stream << packet;

    QHostAddress address;
    node->location.address.ToQHostAddress(&address);
    socket_->writeDatagram(data, address, node->location.port);
    return request;
  }

  void
  DHT::SendPong(const FullTag & fullTag)
  {
    QByteArray data;
    QDataStream stream(data);
    protocol::Header packet;

    packet.id = protocol::Header::Pong;
    packet.tag = fullTag.tag;
    packet.length = 0;
    stream << packet;
    socket_->writeDatagram(data, fullTag.address, fullTag.port);
  }

  void
  DHT::Create()
  {
    if (socket_->state() != QAbstractSocket::BoundState)
      socket_->bind(port_);
  }

  DHTJoinRequest *
  DHT::Join(const QHostAddress & address, quint16 port)
  {
    DHTJoinRequest * request = new DHTJoinRequest(*this, address, port);

    request->tag_ = GenerateTag();
    joinRequests_[request->tag_] = request;
    connect(request, SIGNAL(Joined(DHTJoinRequest *)),
            this, SLOT(Joined(DHTJoinRequest *)));
    request->Join();
    return request;
  }

  void
  DHT::Joined(DHTJoinRequest * /*handler*/)
  {
  }

  void
  DHT::Disconnect()
  {
    // Disconnect from every peers
    socket_->close();
  }

  protocol::Tag
  DHT::GenerateTag()
  {
    protocol::Tag tag;
    quint32       timestamp;
    quint32       r;

    timestamp = time(0);
    r         = qrand();
    tag       = timestamp;
    tag       = (tag << 32) | r;

    return tag;
  }

  FullTag
  DHT::GenerateFullTag(const QHostAddress & addr,
                       quint16              port)
  {
    FullTag full_tag(GenerateTag(), addr, port);

    while (IsTagInUse(full_tag))
      full_tag.tag = GenerateTag();

    return full_tag;
  }

  bool
  DHT::IsTagInUse(const FullTag & tag) const
  {
    Requests_t::const_iterator it = requests_.find(tag);
    return it == requests_.end();
  }
}
