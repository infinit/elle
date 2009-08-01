#include <assert.h>
#include <time.h>
#include <QByteArray>
#include <QDataStream>

#include "hole/DHT.hh"
#include "hole/DHTJoinRequestHandler.hh"

namespace hole
{
  DHT::DHT(QObject * p)
    : QObject(p),
      localNode_(*this),
      socket_(new QUdpSocket(this)),
      nodes_(),
      port_(64626),
      requests_()
  {
    connect(socket_, SIGNAL(readyRead()), this, SLOT(ReadDatagram()));
  }

  DHT::~DHT()
  {
    foreach (Node * node, nodes_)
      delete node;
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
    QByteArray data;
    data.resize(socket_->pendingDatagramSize());
    if (socket_->readDatagram(data.data(), data.size()) <= 0)
      return;

    // decode data
    QDataStream stream(data);
    quint16 cmd_id, tag;
    stream >> cmd_id >> tag;
  }

  void
  DHT::Create()
  {
    if (socket_->state() != QAbstractSocket::BoundState)
      socket_->bind(port_);
  }

  void
  DHT::Join(DHTJoinRequest * request)
  {
    assert(request);
    DHTJoinRequestHandler * handler = new DHTJoinRequestHandler(*this, *request);
    handler->tag_ = GenerateTag();
    requests_[handler->tag_] = handler;
    connect(handler, SIGNAL(Joined(DHTJoinRequestHandler *)),
            this, SLOT(Joined(DHTJoinRequestHandler *)));
    handler->Join();
  }

  void
  DHT::Joined(DHTJoinRequestHandler * handler)
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

    do {
      timestamp = time(0);
      r         = qrand();
      tag       = timestamp;
      tag       = (tag << 32) | r;
    } while (IsTagInUse(tag));

    return tag;
  }

  bool
  DHT::IsTagInUse(protocol::Tag tag) const
  {
    requests_t::const_iterator it = requests_.find(tag);
    return it == requests_.end();
  }
}
