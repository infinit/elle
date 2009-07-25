#ifndef HOLE_DHTJOINREQUESTHANDLER_HH
# define HOLE_DHTJOINREQUESTHANDLER_HH

# include <QTcpSocket>

namespace hole
{
  class DHTJoinRequest;

  class DHTJoinRequestHandler : public QObject
  {
    Q_OBJECT;

  public:
    DHTJoinRequestHandler(DHTJoinRequest & rq, QObject * parent = 0);
    void Join();

    DHTJoinRequest & request_;

  protected slots:
    void SocketConnected();
    void SocketDisconnected();
    void SocketError(QAbstractSocket::SocketError socketError);

  protected:
    QTcpSocket * socket_;
  };
}

#endif /* !HOLE_DHTJOINREQUESTHANDLER_HH */
