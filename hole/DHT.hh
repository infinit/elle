#ifndef HOLE_DHT_HH
# define HOLE_DHT_HH

# include <QObject>

# include "hole/LocalNode.hh"

namespace hole
{
  class DHT : public QObject
  {
    Q_OBJECT;

  public:
    DHT(QObject * parent = 0);
    ~DHT();

    void Get(const Key & key);
    void Put(const Key & key, const QByteArray & data);
    void Join(const QHostAddress & address, quint16 port);
    void Create();

  public slots:
    void Disconnect();

  signals:
    void JoinSucceed();
    void JoinFailed();

    void Disconnected();
    void NodeDisconnected(Node * node);

    void GetSucceed(const Key & key, const QByteArray & data);
    void GetFailed(const Key & key);
    void PutSucceed(const Key & key);
    void PutFailed(const Key & key);

  private:
    LocalNode         localNode_;
    QMap<Key, Node *> nodes_;
  };
}

#endif /* !HOLE_DHT_HH */
