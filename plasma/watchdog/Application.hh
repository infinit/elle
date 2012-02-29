//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix   [Wed 01 Feb 2012 06:57:42 PM CET]
//

#ifndef PLASMA_WATCHDOG_APPLICATION_HH
#define PLASMA_WATCHDOG_APPLICATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <QApplication>


namespace plasma
{
  namespace watchdog
  {

    class LocalServer;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// watchdog application
    ///
    class Application : public QApplication
    {
      Q_OBJECT

    private:
      LocalServer*    _server;

    public:
      Application(int ac, char* av[]);
      ~Application();

      /// Start main loop
      int Exec();
    };

  }
}


#endif /* ! APPLICATION_HH */


