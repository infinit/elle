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

    ///
    /// Store all local client commands. This class acts as a god interface
    /// between components.
    ///
    /// Each command is encoded in JSon format as a dictionary. It has to
    /// contain a "command" key, that contains the name of the command and
    /// the key "_id" which is watchdog id. Any other key/value pair is allowed
    /// and may represent the command arguments.
    ///
    /// The is also a JSon dictionary that contains a "success" key, and
    /// when its value (a boolean) is false, then the response also contains
    /// the "error" key, which is a usefull descriptive string ;)
    ///
    /// Commands are described below with their own callback.
    ///
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
    public:
      QString const& watchdogId() const { return this->_watchdogId; }
      void watchdogId(QString const& id) { this->_watchdogId = id; }

    private:
      ///
      /// Run command is fired by the updater, it gives the token.
      ///   Q: {
      ///       "command": "run",
      ///       "_id": "watchdog id",
      ///       "token": "meta token",
      ///   }
      ///
      ///   R: no response
      ///
      void _OnRun(Connection& conn, Client& client, QVariantMap const& args);


      ///
      /// Stop completely the watchdog.
      ///   Q: {
      ///       "command": "stop",
      ///       "_id": "watchdog id",
      ///   }
      ///
      ///   R: no response
      ///
      void _OnStop(Connection& conn, Client& client, QVariantMap const& args);
    };

  }
}



#endif /* ! CLIENTACTIONS_HH */


