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
    friend class DHT;

  public:
    virtual ~DHTRequest();
    virtual void done();
    virtual void failed();
  };

  class DHTJoinRequest : public DHTRequest
  {
    Q_OBJECT;

  public:
    QHostAddress address;
    quint16      port;

    /* The following fields are used by the DHT */
  protected slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);

  protected:
    QTcpSocket * socket;
  };

  class DHTDataRequest : public DHTRequest
  {
  public:
    Key        key;
    QByteArray data;
  };
}

#endif /* !HOLE_DHTREQUEST_HH */
