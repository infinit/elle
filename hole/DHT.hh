#ifndef HOLE_DHT_HH
# define HOLE_DHT_HH

# include <QObject>
# include <QUdpSocket>

# include "hole/LocalNode.hh"
# include "hole/protocol/Header.hh"

namespace hole
{
  class DHTRequest;
  class DHTRequestHandler;
  class DHTDataRequest;
  class DHTJoinRequest;
  class DHTJoinRequestHandler;

  class DHT : public QObject
  {
    Q_OBJECT;

    friend class DHTRequestHandler;
    friend class DHTJoinRequestHandler;

  public:
    DHT(QObject * parent = 0);
    ~DHT();

    void Port(quint16 port);
    quint16 Port() const;

    void Join(DHTJoinRequest * request);
    void Create();

    void Get(DHTDataRequest * request);
    void Put(DHTDataRequest * request);

    protocol::Tag GenerateTag();

  public slots:
    void Disconnect();

  protected slots:
    void ReadDatagram();
    void Joined(DHTJoinRequestHandler * handler);

  signals:
    void JoinSucceed();
    void JoinFailed();

    void Disconnected();
    void NodeDisconnected(Node * node);

  private:
    bool IsTagInUse(protocol::Tag tag) const;

    typedef QMap<protocol::Tag, DHTRequestHandler *> requests_t;

    LocalNode         localNode_;
    QUdpSocket *      socket_;
    QMap<Key, Node *> nodes_;
    quint16           port_;
    requests_t        requests_;
  };
}

#endif /* !HOLE_DHT_HH */
