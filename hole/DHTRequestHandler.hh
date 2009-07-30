#ifndef HOLE_DHTREQUESTHANDLER_HH
# define HOLE_DHTREQUESTHANDLER_HH

# include <QObject>

# include "hole/protocol/Header.hh"

namespace hole
{
  class DHTRequestHandler : public QObject
  {
  public:
    DHTRequestHandler(QObject * parent = 0);

    protocol::Tag tag;
  };
}

#endif /* !HOLE_DHTREQUESTHANDLER_HH */
