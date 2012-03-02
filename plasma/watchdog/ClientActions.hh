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
      QString     _token;
      QString     _watchdogId;

    public:
      ClientActions(Manager& manager);
      ~ClientActions();

    // properties
      QString const& watchdogId() const { return this->_watchdogId; }
      void watchdogId(QString const& id) { this->_watchdogId = id; }

    private:
      void _OnRun(Connection& conn, Client& client, QVariantMap const& args);
      void _OnStop(Connection& conn, Client& client, QVariantMap const& args);
    };

  }
}



#endif /* ! CLIENTACTIONS_HH */


