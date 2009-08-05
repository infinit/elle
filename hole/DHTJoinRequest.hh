#ifndef HOLE_DHTJOINREQUEST_HH
# define HOLE_DHTJOINREQUEST_HH

# include <QTcpSocket>

# include "hole/DHTRequest.hh"

namespace hole
{
  class DHTJoinRequest : public DHTRequest
  {
    Q_OBJECT;

    friend class DHT;

  public:
    DHTJoinRequest(DHT & dht, const QHostAddress & address, quint16 port);

  signals:
    void Joined(DHTJoinRequest * handler);

  protected:
    void Join();

    QHostAddress address_;
    quint16      port_;
  };
}

#endif /* !HOLE_DHTJOINREQUEST_HH */
