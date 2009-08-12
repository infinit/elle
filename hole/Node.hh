#ifndef HOLE_NODE_HH
# define HOLE_NODE_HH

# include <QHostAddress>

# include "hole/Key.hh"
# include "hole/FreeList.hh"
# include "hole/network/Location.hh"

namespace hole
{
  class DHT;

  class Node : public FreeList<Node>
  {
  public:
    network::Location location;
    Key               key;
  };
}

#endif /* !HOLE_NODE_HH */
