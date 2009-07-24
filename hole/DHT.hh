#ifndef HOLE_DHT_HH
# define HOLE_DHT_HH

# include <QObject>

# include "hole/LocalNode.hh"

namespace hole
{
  class DHTDataRequest;
  class DHTJoinRequest;

  class DHT : public QObject
  {
    Q_OBJECT;

  public:
    DHT(QObject * parent = 0);
    ~DHT();

    void Port(quint16 port);
    quint16 Port() const;

    void Join(DHTJoinRequest * request);
    void Create();

    void Get(DHTDataRequest * request);
    void Put(DHTDataRequest * request);

  public slots:
    void Disconnect();

  signals:
    void JoinSucceed();
    void JoinFailed();

    void Disconnected();
    void NodeDisconnected(Node * node);

  private:
    LocalNode         localNode_;
    QMap<Key, Node *> nodes_;
  };
}

#endif /* !HOLE_DHT_HH */
