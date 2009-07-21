#ifndef HOLE_NODE_HH
# define HOLE_NODE_HH

# include <QHostAddress>

# include "hole/Key.hh"

namespace hole
{
  class Node : public QObject
  {
  public:
    Key          key;
    QHostAddress address;
    quint16      port;
  };
}

#endif /* !HOLE_NODE_HH */
