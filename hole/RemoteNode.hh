#ifndef HOLE_REMOTE_NODE_HH
# define HOLE_REMOTE_NODE_HH

# include <QTcpSocket>

# include "hole/Node.hh"

namespace hole
{
  class RemoteNode : public Node
  {
    QTcpSocket socket;
  };
}

#endif /* !HOLE_REMOTE_NODE_HH */
