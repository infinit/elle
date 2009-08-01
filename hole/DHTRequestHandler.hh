#ifndef HOLE_DHTREQUESTHANDLER_HH
# define HOLE_DHTREQUESTHANDLER_HH

# include <QObject>

# include "hole/protocol/Header.hh"

namespace hole
{
  class DHT;

  class DHTRequestHandler : public QObject
  {
  public:
    DHTRequestHandler(DHT & dht);

    DHT &         dht_;
    protocol::Tag tag_;
  };
}

#endif /* !HOLE_DHTREQUESTHANDLER_HH */
