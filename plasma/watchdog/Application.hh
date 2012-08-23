#ifndef PLASMA_WATCHDOG_APPLICATION_HH
# define PLASMA_WATCHDOG_APPLICATION_HH

# include <QCoreApplication>

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
      LocalServer*    _server;

    public:
      Application(int ac, char* av[]);
      ~Application();

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
