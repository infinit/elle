//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix   [Sun 04 Mar 2012 08:56:11 AM CET]
//

#ifndef PLASMA_WATCHDOG_NETWORKMANAGER_HH
# define PLASMA_WATCHDOG_NETWORKMANAGER_HH

//
// ---------- includes --------------------------------------------------------
//

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
//
// ---------- classes ---------------------------------------------------------
//

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

      void UpdateNetworks();

    private:
      void _OnNetworksUpdate(meta::NetworksResponse const& response);
      void _OnNetworkUpdate(meta::NetworkResponse const& response);
    };

  }
}


//
// ---------- templates -------------------------------------------------------
//

#endif /* ! NETWORKMANAGER_HH */


