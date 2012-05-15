//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphael Londeix   [Wed 29 Feb 2012 12:43:33 PM CET]
//

#ifndef PLASMA_WATCHDOG_LOCALSERVER_HH
# define PLASMA_WATCHDOG_LOCALSERVER_HH

//
// ---------- includes --------------------------------------------------------
//

# include <memory>

# include <QCoreApplication>
# include <QLocalServer>

namespace plasma
{
  namespace watchdog
  {
      class Manager;
      class Connection;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// The local server binds whatever local socket in a portable way (thx Qt)
    /// and binds to it. It also stores and manages all client connections.
    ///
    class LocalServer : public QLocalServer
    {
      Q_OBJECT

    public:
      enum class State { Stopped, Running };
    private:
      typedef std::shared_ptr<Connection> ConnectionPtr;

    private:
      State                         _state;
      std::unique_ptr<Manager>      _manager;

    public:
      LocalServer(QCoreApplication& app);
      ~LocalServer();
      void Start(std::string const& watchdogId);

    private:
      void _HandleNewConnection(ConnectionPtr& newConnection);
      void _OnClientError(ConnectionPtr conn, std::string const& error);
      void _OnClientCommand(ConnectionPtr conn, QByteArray const& data);

    private Q_SLOTS:
      void _OnNewConnection();

    };


  }
}

#endif /* ! LOCALSERVER_HH */


