#ifndef  PLASMA_WATCHDOG_NETWORKMANAGER_HH
# define PLASMA_WATCHDOG_NETWORKMANAGER_HH

# include <list>
# include <map>
# include <memory>
# include <string>

# include <QApplication>

namespace plasma
{
  namespace watchdog
  {

    class InfinitNetwork;
    class Manager;

    namespace meta = ::plasma::metaclient;

    ///
    /// This class keep the list of infinit networks instance.
    /// @see InfinitNetwork.hh
    ///
    class NetworkManager
    {
    private:
      typedef std::unique_ptr<InfinitNetwork> InfinitNetworkPtr;
      typedef std::map<std::string, InfinitNetworkPtr> NetworkMap;

    private:
      Manager&        _manager;
      NetworkMap      _networks;

    public:
      NetworkManager(Manager& manager);

      void update_networks();
      void stop();

      NetworkMap const& networks() const;

    private:
      void _on_networks_update(meta::NetworksResponse const& response);
      void _on_network_update(meta::NetworkResponse const& response);
    };

  }
}

#endif /* ! NETWORKMANAGER_HH */
