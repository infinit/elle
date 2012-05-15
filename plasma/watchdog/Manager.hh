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

# include <QCoreApplication>
# include <QVariantMap>
# include <QVariantList>

# include "plasma/metaclient/MetaClient.hh"

namespace plasma
{
  namespace watchdog
  {

    class Connection;
    class Client;
    class ClientActions;
    class NetworkManager;

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

      typedef plasma::metaclient::MetaClient MetaClient;

    private:
      QCoreApplication&   _app;
      ClientMap*          _clients;
      CommandMap*         _commands;
      ClientActions*      _actions;
      NetworkManager*     _networkManager;
      MetaClient          _meta;
      std::string         _identity;

    public:

      /// ctor & dtor
      Manager(QCoreApplication& app);
      ~Manager();

      /// properties
      void token(QByteArray const& token);
      void token(QString const& token)        { this->token(token.toAscii()); }

      MetaClient& meta()                                { return this->_meta; }

      NetworkManager& networkManager()       { return *this->_networkManager; }

      std::string const& identity() const           { return this->_identity; }
      void identity(std::string const& id)            { this->_identity = id; }
      void identity(QString const& id)  { this->_identity = id.toStdString(); }

      /// Called from the LocalServer to add a new connection
      Client& RegisterConnection(ConnectionPtr& conn);
      void UnregisterConnection(ConnectionPtr& conn);

      ///
      /// Used by ClientActions to register callbacks. Any class may
      /// use it, but only one hook per command is allowed.
      ///
      void RegisterCommand(std::string const& id, Command cmd);
      void UnregisterCommand(std::string const& id);
      void UnregisterAllCommands();

      /// Dispatch a command from a connection
      void ExecuteCommand(ConnectionPtr& conn, QVariantMap const& cmd);

      ///
      /// The manager is started from the class Application, but can
      /// be stopped anywhere.
      ///
      void Start(std::string const& watchdogId);
      void Stop();
    };

  }
}



#endif /* ! MANAGER_HH */


