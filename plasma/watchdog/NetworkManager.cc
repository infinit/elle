//
// ---------- header ----------------------------------------------------------
//
// project       @PROJECT@
//
// license       infinit
//
// author        Raphael Londeix   [Sun 04 Mar 2012 08:58:42 AM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <iostream>
#include <set>
#include <stdexcept>

#include "InfinitNetwork.hh"
#include "Manager.hh"
#include "NetworkManager.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

NetworkManager::NetworkManager(Manager& manager):
  _manager(manager),
  _networks()
{
}

//
// ---------- methods  --------------------------------------------------------
//

void NetworkManager::Stop()
{
  this->_networks.clear();
}

void NetworkManager::UpdateNetworks()
{
  using namespace std::placeholders;
  this->_manager.meta().GetNetworks(
      std::bind(&NetworkManager::_OnNetworksUpdate, this, _1)
  );
}
void NetworkManager::_OnNetworksUpdate(meta::NetworksResponse const& response)
{
  std::set<std::string> visited;

  // apply to the networks
    {
      auto it =  response.networks.begin(),
           end = response.networks.end();
      for (; it != end; ++it)
        {
          visited.insert(*it);
          using namespace std::placeholders;
          this->_manager.meta().GetNetwork(
              *it,
              std::bind(&NetworkManager::_OnNetworkUpdate, this, _1)
          );
        }
    }

  // remove old networks
  // XXX Call a cleanup function ?
    {
      auto it =  this->_networks.begin(),
           end = this->_networks.end();
      for (; it != end; ++it)
        {
          if (visited.find(it->first) == visited.end())
            {
              this->_networks.erase(it);
              end = this->_networks.end();
            }
        }
    }
}

void NetworkManager::_OnNetworkUpdate(meta::NetworkResponse const& r)
{
  std::cout << "UPDATING NETWORK !\n";
  auto it = this->_networks.find(r._id);
  if (it == this->_networks.end())
    {
      auto res = this->_networks.insert(
          NetworkMap::value_type(
            r._id,
            InfinitNetworkPtr(new InfinitNetwork(this->_manager, r))
          )
      );
      if (res.second == false)
        throw std::runtime_error("Out of memory");
    }
  else
    it->second->Update(r);
}
