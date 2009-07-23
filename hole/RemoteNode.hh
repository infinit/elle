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

    virtual QHostAddress Address() const;
    virtual quint16 Port() const;
    virtual void Disconnect();

  private:
    QTcpSocket socket_;
  };
}

#endif /* !HOLE_REMOTE_NODE_HH */
