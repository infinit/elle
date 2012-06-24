#ifndef  PLASMA_WATCHDOG_LOCALSERVER_HH
# define PLASMA_WATCHDOG_LOCALSERVER_HH

# include <memory>

# include <QCoreApplication>
# include <QLocalServer>

namespace plasma
{
  namespace watchdog
  {
    class Manager;
    class Connection;

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
      void start(std::string const& watchdogId);

    private:
      void _handle_new_connection(ConnectionPtr& newConnection);
      void _on_client_error(ConnectionPtr conn, std::string const& error);
      void _on_client_command(ConnectionPtr conn, QByteArray const& data);

    private Q_SLOTS:
      void _on_new_connection();
    };

  }
}

#endif /* ! LOCALSERVER_HH */
