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

# include <QLocalSocket>

namespace plasma
{
  namespace watchdog
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Connection : QObject
    {
      Q_OBJECT

    public:
      typedef std::unique_ptr<QLocalSocket> QLocalSocketPtr;
      typedef std::function<void(std::string const&)> Errback;
      typedef std::function<void(QByteArray const&)> Cmdback;

    private:
      QLocalSocketPtr   _socket;
      Errback           _errback;
      Cmdback           _cmdback;

    public:
      Connection(QLocalSocketPtr&& socket, QObject* parent = nullptr);
      ~Connection();

    public:
      void Connect(Cmdback cmdback, Errback errback);

    private:
      void _Raise(std::string const& error);

    private slots:
      void _OnDisconnect();
      void _OnReadyRead();
    };

  }
}


#endif /* ! CONNECTION_HH */


