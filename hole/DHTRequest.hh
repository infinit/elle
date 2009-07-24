#ifndef HOLE_DHTREQUEST_HH
# define HOLE_DHTREQUEST_HH

# include <QByteArray>
# include <QHostAddress>

# include "hole/Key.hh"

namespace hole
{
  struct DHTRequest
  {
    virtual ~DHTRequest();
    virtual void done();
    virtual void failed();
  };

  struct DHTJoinRequest : public DHTRequest
  {
    QHostAddress address;
    quint16      port;
  };

  struct DHTDataRequest : public DHTRequest
  {
    Key        key;
    QByteArray data;
  };
}

#endif /* !HOLE_DHTREQUEST_HH */
