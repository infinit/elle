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
    void Bootstrap(const QHostAddress & address, quint16 port);

  public slots:
    void Disconnect();
    void NodeDisconnected(Node * node);

  signals:
    void BootstrapSucceed();
    void BootstrapFailed();
    void GetSucceed(const Key & key, const QByteArray & data);
    void GetFailed(const Key & key);
    void PutSucceed(const Key & key);
    void PutFailed(const Key & key);

  private:
    LocalNode         localNode;
    QMap<Key, Node *> nodes;
  };
}

#endif /* !HOLE_DHT_HH */
