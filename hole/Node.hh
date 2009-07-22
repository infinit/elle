#ifndef HOLE_NODE_HH
# define HOLE_NODE_HH

# include <QHostAddress>

# include "hole/Key.hh"

namespace hole
{
  class DHT;

  class Node : public QObject
  {
  public:
    Node(DHT & dht);
    virtual ~Node();

    DHT &        dht_;
    Key          key_;
    QHostAddress address_;
    quint16      port_;
  };
}

#endif /* !HOLE_NODE_HH */
