#ifndef HOLE_REMOTE_NODE_HH
# define HOLE_REMOTE_NODE_HH

# include <QTcpSocket>

# include "hole/FreeList.hh"
# include "hole/Node.hh"

namespace hole
{
  class RemoteNode : public Node,
                     public FreeList<RemoteNode>
  {
    Q_OBJECT;

  public:
    RemoteNode(DHT & dht);

  private:
    QTcpSocket socket;
  };
}

#endif /* !HOLE_REMOTE_NODE_HH */
