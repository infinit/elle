#ifndef PLASMA_WATCHDOG_APPLICATION_HH
# define PLASMA_WATCHDOG_APPLICATION_HH

# include "LocalServer.hh"

# include <QCoreApplication>

# include <memory>

namespace plasma
{
  namespace watchdog
  {

    class LocalServer;

    ///
    /// watchdog application
    ///
    class Application : public QCoreApplication
    {
      Q_OBJECT

    private:
        std::unique_ptr<LocalServer> _server;

    public:
      Application(std::string const& user_id);

      /// Start main loop
      int exec();

      // handle exception
      bool
      notify(QObject* rec,
             QEvent* ev);
    };

  }
}

#endif /* ! APPLICATION_HH */
