#ifndef HOLE_LOCAL_NODE_HH
# define HOLE_LOCAL_NODE_HH

# include <QTcpServer>

# include "hole/Node.hh"

namespace hole
{
  class LocalNode : public Node
  {
    Q_OBJECT;

  public:
    LocalNode(DHT & dht);

    void Listen(quint16 port);

    virtual QHostAddress Address() const;
    virtual quint16 Port() const;
    virtual void Disconnect();

  protected slots:
    void IncomingConnection();

  private:
    QTcpServer server_;
  };
}

#endif /* !HOLE_LOCAL_NODE_HH */
