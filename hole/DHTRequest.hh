#ifndef HOLE_DHTREQUEST_HH
# define HOLE_DHTREQUEST_HH

# include <QByteArray>
# include <QHostAddress>

# include "hole/Key.hh"

namespace hole
{
  class DHT;
  class QTcpSocket;

  class DHTRequest : public QObject
  {
    Q_OBJECT;

  public:
    DHTRequest(DHT & dht);
    virtual ~DHTRequest();

  signals:
    /** called when the request is finished and successful */
    void Done();
    /** called when the request failed */
    void Failed();

  protected:
    DHT &         dht_;
    protocol::Tag tag_;
  };

  class DHTDataRequest : public DHTRequest
  {
  public:
    Key        key;
    QByteArray data;
  };
}

#endif /* !HOLE_DHTREQUEST_HH */
