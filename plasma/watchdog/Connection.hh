#ifndef  PLASMA_WATCHDOG_CONNECTION_HH
# define PLASMA_WATCHDOG_CONNECTION_HH

# include <functional>
# include <memory>

# include <boost/noncopyable.hpp>

# include <QLocalSocket>

namespace plasma
{
  namespace watchdog
  {

    ///
    /// A local client connection. It provides a layer between raw data from
    /// the socket and the client, which handles the packet.
    ///
    class Connection
      : public QObject
      , private boost::noncopyable
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

    protected:
      using QObject::connect;
    public:
      /// Gives to the connection the callback and errback.
      void connect(Cmdback cmdback, Errback errback);

    private:
      void _raise(std::string const& error);

    private Q_SLOTS:
      void _on_disconnect();
      void _on_ready_read();
    };

  }
}


#endif /* ! CONNECTION_HH */


