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
#include <QLocalServer>

namespace plasma
{
  namespace watchdog
  {

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
      QLocalServer*   _server;
    public:
      Application(int ac, char* av[]);
      ~Application();

      /// Start main loop
      int Exec();
    };

  }
}


#endif /* ! APPLICATION_HH */


