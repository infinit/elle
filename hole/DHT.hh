#ifndef HOLE_DHT_HH
# define HOLE_DHT_HH

# include <QObject>
# include <QUdpSocket>

# include "hole/Node.hh"
# include "hole/FingerTable.hh"
# include "hole/FullTag.hh"
# include "hole/protocol/Header.hh"

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

    DHTRequest * Ping(Node * node);

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

    void HandleFindSuccessor(const FullTag & fullTag,
                             const QByteArray & data);

    /** Used to reply to Ping. \sa Ping(const Key &) */
    void SendPong(const FullTag & fullTag);

    typedef QMap<FullTag, DHTRequest *> Requests_t;
    typedef QMap<protocol::Tag, DHTJoinRequest *> JoinRequests_t;

    Node           localNode_;
    FingerTable    fingerTable_;
    QUdpSocket *   socket_;
    quint16        port_;
    Requests_t     requests_;
    JoinRequests_t joinRequests_;
  };
}

#endif /* !HOLE_DHT_HH */
