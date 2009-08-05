#ifndef HOLE_DHT_HH
# define HOLE_DHT_HH

# include <QObject>
# include <QUdpSocket>

# include "hole/LocalNode.hh"
# include "hole/protocol/Header.hh"
# include "hole/FullTag.hh"

namespace hole
{
  class DHTRequest;
  class DHTDataRequest;
  class DHTJoinRequest;

  class DHT : public QObject
  {
    Q_OBJECT;

    friend class DHTRequest;
    friend class DHTJoinRequest;

  public:
    DHT(QObject * parent = 0);
    ~DHT();

    void Port(quint16 port);
    quint16 Port() const;

    /** Join an existing network */
    DHTJoinRequest * Join(const QHostAddress & address, quint16 port);
    void Create();

    void Get(DHTDataRequest * request);
    void Put(DHTDataRequest * request);

    DHTRequest * Ping(const Key & key);

    protocol::Tag GenerateTag();
    FullTag GenerateFullTag(const QHostAddress & addr,
                            quint16              port);

  public slots:
    void Disconnect();

  protected slots:
    void ReadDatagram();
    void Joined(DHTJoinRequest * handler);

  signals:
    void JoinSucceed();
    void JoinFailed();

    void Disconnected();
    void NodeDisconnected(Node * node);

  private:
    bool IsTagInUse(const FullTag & tag) const;
    void HandleCommand(const QByteArray & data,
                       protocol::CmdId    cmdId,
                       const FullTag &    fullTag);

    /** Used to reply to Ping. \sa Ping(const Key &) */
    void SendPong(const FullTag & fullTag);

    typedef QMap<FullTag, DHTRequest *> requests_t;
    typedef QMap<Key, Node *> nodes_t;

    LocalNode    localNode_;
    QUdpSocket * socket_;
    nodes_t      nodes_;
    quint16      port_;
    requests_t   requests_;
  };
}

#endif /* !HOLE_DHT_HH */
