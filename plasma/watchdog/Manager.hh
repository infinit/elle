//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix [Thu 01 Mar 2012 11:07:00 AM CET]
//

#ifndef PLASMA_WATCHDOG_MANAGER_HH
# define PLASMA_WATCHDOG_MANAGER_HH

//
// ---------- includes --------------------------------------------------------
//

# include <unordered_map>
# include <memory>
# include <functional>

# include <QApplication>
# include <QVariantMap>
# include <QVariantList>

namespace plasma
{
  namespace watchdog
  {

    class Connection;
    class Client;
    class ClientActions;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// The manager dispatch received command and stores clients with
    /// and their connection.
    ///
    class Manager
    {
    public:
      typedef std::shared_ptr<Connection> ConnectionPtr;
      typedef std::unique_ptr<Client> ClientPtr;
      typedef std::unique_ptr<ClientActions> ClientActionsPtr;
      typedef std::unordered_map<ConnectionPtr, ClientPtr> ClientMap;

      typedef std::function<void(Connection&, Client&, QVariantMap const&)> Command;
      typedef std::unordered_map<std::string, Command> CommandMap;

    private:
      QApplication&   _app;
      ClientMap*      _clients;
      CommandMap*     _commands;
      ClientActions*  _actions;
      Client*         _admin;

    public:

      /// ctor & dtor
      Manager(QApplication& app);
      ~Manager();

      /// properties
      void token(QByteArray const& token);
      void token(QString const& token) { this->token(token.toAscii()); }

      /// methods
      Client& RegisterConnection(ConnectionPtr& conn);
      void UnregisterConnection(ConnectionPtr& conn);

      void RegisterCommand(std::string const& id, Command cmd);
      void UnregisterCommand(std::string const& id);
      void UnregisterAllCommands();
      void ExecuteCommand(ConnectionPtr& conn, QVariantMap const& cmd);

      void Start(std::string const& watchdogId);
      void Stop();

      void RefreshNetworks();

    private:
      void _OnNetworksUpdated();
    };

  }
}



#endif /* ! MANAGER_HH */


