#ifndef HOLE_DHTREQUEST_HH
# define HOLE_DHTREQUEST_HH

# include <QByteArray>
# include <QHostAddress>

# include "hole/Key.hh"

namespace hole
{
  class DHT;
  class QTcpSocket;

  class DHTRequest
  {
  public:
    virtual ~DHTRequest();

    /** called when the request is finished and successful */
    virtual void Done();
    /** called when the request failed */
    virtual void Failed();
  };

  class DHTJoinRequest : public DHTRequest
  {
  public:
    QHostAddress address;
    quint16      port;
  };

  class DHTDataRequest : public DHTRequest
  {
  public:
    Key        key;
    QByteArray data;
  };
}

#endif /* !HOLE_DHTREQUEST_HH */
