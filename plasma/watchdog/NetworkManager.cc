#include <iostream>
#include <set>
#include <stdexcept>

#include "InfinitNetwork.hh"
#include "Manager.hh"
#include "NetworkManager.hh"

using namespace plasma::watchdog;

NetworkManager::NetworkManager(Manager& manager):
  _manager(manager),
  _networks()
{
}

void NetworkManager::stop()
{
  this->_networks.clear();
}

void NetworkManager::update_networks()
{
  using namespace std::placeholders;
  this->_manager.meta().Networks(
      std::bind(&NetworkManager::_on_networks_update, this, _1)
  );
}
void NetworkManager::_on_networks_update(meta::NetworksResponse const& response)
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
          this->_manager.meta().Network(
              *it,
              std::bind(&NetworkManager::_on_network_update, this, _1)
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

void NetworkManager::_on_network_update(meta::NetworkResponse const& r)
{
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
    it->second->update(r);
}
