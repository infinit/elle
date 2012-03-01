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

# include <QApplication>
# include <QVariantMap>

namespace plasma
{
  namespace watchdog
  {

    class Connection;
    class Client;

//
// ---------- classes ---------------------------------------------------------
//

    class Manager
    {
    public:
      typedef std::shared_ptr<Connection> ConnectionPtr;
      typedef std::unique_ptr<Client> ClientPtr;
      typedef std::unordered_map<ConnectionPtr, ClientPtr> ClientMap;

    private:
      QApplication& _app;
      ClientMap* _clients;

    public:
      Manager(QApplication& app);
      ~Manager();
      Client& RegisterConnection(ConnectionPtr& conn);
      void UnregisterConnection(ConnectionPtr& conn);
      void ExecuteCommand(ConnectionPtr& conn, QVariantMap const& cmd);
    };

  }
}



#endif /* ! MANAGER_HH */


