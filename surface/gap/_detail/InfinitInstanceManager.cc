#include "InfinitInstanceManager.hh"

#include <common/common.hh>

#include <elle/Exception.hh>
#include <elle/log.hh>
#include <elle/memory.hh>

ELLE_LOG_COMPONENT("infinit.surface.gap.InfinitInstanceManager");

namespace surface
{
  namespace gap
  {

    InfinitInstanceManager::InfinitInstanceManager(std::string const& user_id)
      : _user_id{user_id}
    {}

    void
    InfinitInstanceManager::launch_network(std::string const& network_id)
    {
      ELLE_TRACE("Starting network process of %s", network_id);
      if (_instances.find(network_id) != _instances.end())
        throw elle::Exception{"Network " + network_id + " already launched"};
      auto process = elle::make_unique<elle::system::Process>(
        common::infinit::binary_path("8infinit"),
        std::list<std::string>{"-n", network_id, "-u", _user_id}
      );
      _instances[network_id].reset(new InfinitInstance{
          network_id,
          "",
          std::move(process),
      });
    }

    void
    InfinitInstanceManager::stop_network(std::string const& network_id)
    {
      ELLE_TRACE("stoping newtork");

      try
      {
        auto const& instance = this->network_instance(network_id);
        instance.process->terminate();
        instance.process->wait();
      }
      catch (elle::Exception const& e)
      {
        ELLE_DEBUG("no network found, no infinit to kill");
        return;
      }

      _instances.erase(network_id);
    }

    InfinitInstance const&
    InfinitInstanceManager::network_instance(std::string const& network_id) const
    {
      auto it = _instances.find(network_id);
      if (it == _instances.end())
        throw elle::Exception{"Cannot find any network " + network_id};
      return *(it->second);
    }

    InfinitInstance const*
    InfinitInstanceManager::network_instance_for_file(std::string const& path)
    {
      for (auto const& pair : _instances)
        {
          std::string mount_point = pair.second->mount_point;

          if (path.compare(0, mount_point.length(), path) != 0)
            return pair.second.get();
        }
      return nullptr;
    }

  }
}
