//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphael Londeix   [Wed 29 Feb 2012 02:02:31 PM CET]
//

#ifndef PLASMA_WATCHDOG_CONNECTION_HH
# define PLASMA_WATCHDOG_CONNECTION_HH

//
// ---------- includes --------------------------------------------------------
//

# include <functional>
# include <memory>

# include <boost/noncopyable.hpp>

# include <QLocalSocket>

namespace plasma
{
  namespace watchdog
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// A local client connection. It provides a layer between raw data from
    /// the socket and the client, which handles the packet.
    ///
    class Connection : public QObject, private boost::noncopyable
    {
      Q_OBJECT

    public:
      typedef std::unique_ptr<QLocalSocket> QLocalSocketPtr;
      typedef std::function<void(std::string const&)> Errback;

      /// This is the way the connection gives back a network
      /// packet.
      typedef std::function<void(QByteArray const&)> Cmdback;

    private:
      QLocalSocketPtr   _socket;
      Errback           _errback;
      Cmdback           _cmdback;

    public:
      Connection(QLocalSocketPtr&& socket);
      ~Connection();

    public:
      /// Gives to the connection the callback and errback.
      void Connect(Cmdback cmdback, Errback errback);

    private:
      void _Raise(std::string const& error);

    private Q_SLOTS:
      void _OnDisconnect();
      void _OnReadyRead();
    };

  }
}


#endif /* ! CONNECTION_HH */


