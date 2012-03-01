//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix   [Thu 01 Mar 2012 12:53:50 PM CET]
//

#ifndef PLASMA_WATCHDOG_CLIENTACTIONS_HH
# define PLASMA_WATCHDOG_CLIENTACTIONS_HH

//
// ---------- includes --------------------------------------------------------
//

# include <QVariantList>

namespace plasma
{
  namespace watchdog
  {
    class Manager;
    class Client;
    class Connection;

//
// ---------- classes ---------------------------------------------------------
//

    /// Store all client commands
    class ClientActions
    {
    private:
      Manager&    _manager;

    public:
      ClientActions(Manager& manager);

    private:
      void _OnRun(Connection& conn, Client& client, QVariantList const& args);
    };

  }
}



#endif /* ! CLIENTACTIONS_HH */


