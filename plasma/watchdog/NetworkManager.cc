#include <iostream>
#include <set>
#include <stdexcept>

#include <elle/log.hh>

#include "InfinitNetwork.hh"
#include "Manager.hh"
#include "NetworkManager.hh"

ELLE_LOG_COMPONENT("infinit.plasma.watchdog");

using namespace plasma::watchdog;

NetworkManager::NetworkManager(Manager& manager):
  _manager(manager),
  _networks()
{
}

void NetworkManager::stop()
{
  for (auto& pair : this->_networks)
      pair.second->stop();
}

void NetworkManager::update_networks()
{
  ELLE_DEBUG("Updating networks");
  using namespace std::placeholders;
  this->_on_networks_update(this->_manager.meta().networks());
}

NetworkManager::NetworkMap const& NetworkManager::networks() const
{
  return this->_networks;
}

void NetworkManager::_on_networks_update(meta::NetworksResponse const& response)
{
  std::set<std::string> visited;

  ELLE_DEBUG("Got network list");
  // apply to the networks
    {
      auto it =  response.networks.begin(),
           end = response.networks.end();
      for (; it != end; ++it)
        {
          visited.insert(*it);
          using namespace std::placeholders;
          this->_on_network_update(this->_manager.meta().network(*it));
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
  ELLE_DEBUG("Updating network %s", r._id);
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
      res.first->second->update(r);
    }
  else
    it->second->update(r);
}


