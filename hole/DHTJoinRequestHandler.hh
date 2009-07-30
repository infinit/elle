#ifndef HOLE_DHTJOINREQUESTHANDLER_HH
# define HOLE_DHTJOINREQUESTHANDLER_HH

# include <QTcpSocket>

# include "hole/DHTRequestHandler.hh"

namespace hole
{
  class DHTJoinRequest;

  class DHTJoinRequestHandler : public DHTRequestHandler
  {
    Q_OBJECT;

  public:
    DHTJoinRequestHandler(DHTJoinRequest & rq, QObject * parent = 0);
    void Join();

    DHTJoinRequest & request_;

  signals:
    void Joined(DHTJoinRequestHandler * handler);
  };
}

#endif /* !HOLE_DHTJOINREQUESTHANDLER_HH */
