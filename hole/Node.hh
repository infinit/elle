#ifndef HOLE_NODE_HH
# define HOLE_NODE_HH

# include <QHostAddress>

# include "hole/Key.hh"

namespace hole
{
  class DHT;

  class Node : public QObject
  {
    Q_OBJECT;

  public:
    Node(DHT & dht);
    virtual ~Node();

    virtual QHostAddress Address() const = 0;
    virtual quint16 Port() const = 0;

  public slots:
    virtual void Disconnect() = 0;

  public:
    DHT &        dht_;
    Key          key_;
  };
}

#endif /* !HOLE_NODE_HH */
