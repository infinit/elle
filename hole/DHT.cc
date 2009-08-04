#include <assert.h>
#include <time.h>
#include <QByteArray>
#include <QDataStream>

#include "hole/DHT.hh"
#include "hole/DHTRequest.hh"
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
    /* this switch handles new requests */
    switch (cmdId)
    {
    case protocol::Header::Ping:
      return;
    case protocol::Header::Quit:
      return;
    case protocol::Header::FindSuccessor:
      return;
    }

    /* handle commands with request handlers */
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
    FullTag full_tag(handler->tag_,
                      handler->request_.address,
                      handler->request_.port);
    requests_[full_tag] = handler;
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
    requests_t::const_iterator it = requests_.find(tag);
    return it == requests_.end();
  }
}
