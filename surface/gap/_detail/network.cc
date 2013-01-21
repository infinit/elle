#include "../State.hh"
#include "../MetricReporter.hh"

#include <common/common.hh>

#include <elle/format/json.hh>
#include <elle/idiom/Close.hh>

#include <boost/filesystem.hpp>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>
#include <QProcess>

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

namespace surface
{
  namespace gap
  {

    namespace fs = boost::filesystem;
    namespace json = elle::format::json;

    bool
    State::_wait_portal(std::string const& user_id,
                        std::string const& network_id)
    {
      ELLE_TRACE("_wait_portal for network %s", network_id);
      (void) this->refresh_networks();

      // XXX: threaded "sendfiles" has some strange effect on QLocalSocket...
      // "network status" forces watchdog to update data of
      // (void) this->network_status(network_id);

      ELLE_DEBUG("retrieving portal path");
      auto portal_path = common::infinit::portal_path(user_id, network_id);

      ELLE_DEBUG("portal path is %s", portal_path);

      for (int i = 0; i < 30; ++i)
        {
          ELLE_DEBUG("Waiting for portal.");
          if (fs::exists(portal_path))
            return true;
          ::sleep(1);
        }
      return false;
    }

    std::string
    State::create_network(std::string const& name)
    {
      metrics::google::server().store("network:create:attempt");

      auto response = this->_meta->create_network(name);

      metrics::google::server().store("network:create:succeed");

      this->_networks_dirty = true;
      this->_networks_status_dirty = true;
      this->refresh_networks(); //XXX not optimal
      return response.created_network_id;
    }

    std::string
    State::delete_network(std::string const& network_id, bool force)
    {
      auto const& net = this->_networks.find(network_id);

      if (net != this->_networks.end())
      {
        delete net->second;
        this->_networks.erase(net);
      }

      metrics::google::server().store("network:delete:attempt");

      auto response = this->_meta->delete_network(network_id, force);

      metrics::google::server().store("network:delete:succeed");

      this->_networks_dirty = true;
      this->_networks_status_dirty = true;
      this->refresh_networks(); //XXX not optimal
      return response.deleted_network_id;
    }

    std::map<std::string, Network*> const&
    State::networks()
    {
      if (this->_networks_dirty)
        {
          auto response = this->_meta->networks();
          for (auto const& network_id: response.networks)
            {
              if (this->_networks.find(network_id) == this->_networks.end())
                {
                  auto response = this->_meta->network(network_id);
                  this->_networks[network_id] = new Network{response};
                }
            }
          this->_networks_dirty = false;
        }
      return this->_networks;
    }

    Network const&
    State::network(std::string const& id)
    {
      auto it = this->networks().find(id);
      if (it == this->networks().end())
        throw Exception{
            gap_error,
            "Cannot find any network for id '" + id + "'"
        };
      return *(it->second);
    }

    void
    State::network_add_user(std::string const& network_id,
                            std::string const& user)
    {
      metrics::google::server().store("network:user:add:attempt");

      // makes user we have an id
      std::string user_id = this->user(user)._id;
      auto it = this->networks().find(network_id);
      ELLE_ASSERT(it != this->networks().end());
      Network* network = it->second;
      ELLE_ASSERT(network != nullptr);

      std::string const& group_binary = common::infinit::binary_path("8group");

      QStringList arguments;
      arguments << "--user" << this->_me._id.c_str()
                << "--type" << "user"
                << "--add"
                << "--network" << network->_id.c_str()
                << "--identity" << this->user(user_id).public_key.c_str()
      ;
      ELLE_DEBUG("LAUNCH: %s %s",
                      group_binary,
                      arguments.join(" ").toStdString());
      QProcess p;
      p.start(group_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception(gap_internal_error, "8group binary failed");
      if (p.exitCode())
        throw Exception(gap_internal_error, "8group binary exited with errors");

      auto res = this->_meta->network_add_user(network_id, user_id);
      if (std::find(network->users.begin(),
                    network->users.end(),
                    user_id) == network->users.end())
        network->users.push_back(user_id);

      metrics::google::server().store("network:user:add:succeed");;
    }

    std::map<std::string, NetworkStatus*> const&
    State::networks_status()
    {
      ELLE_TRACE("getting networks status from watchdog");
      if (this->_networks_status_dirty)
        {
          json::Dictionary response;

          ELLE_DEBUG("waiting for response");
          this->_send_watchdog_cmd("status", nullptr, &response);

          ELLE_DEBUG("reading response");
          auto& networks = response["networks"].as_array();

          for (size_t i = 0; i < networks.size(); ++i)
            {
              ELLE_DEBUG("network '%i'.", i);
              auto& network = networks[i].as_dictionary();
              ELLE_DEBUG("> '%s'.", network["_id"].as_string());
              std::string mount_point = network["mount_point"].as_string();
              std::string network_id = network["_id"].as_string();

              NetworkStatus* status = nullptr;
              auto it = this->_networks_status.find(network_id);
              if (it == this->_networks_status.end())
                {
                  status = this->_networks_status[network_id]
                         = new NetworkStatus;
                }
              else
                {
                  status = it->second;
                }
              ELLE_ASSERT(status != nullptr);
              *status = NetworkStatus{
                  network_id,
                  mount_point,
              };
            }
          this->_networks_status_dirty = false;
        }
      else
        {
          ELLE_DEBUG("Networks are clean.");
        }
      return this->_networks_status;
    }

    NetworkStatus const&
    State::network_status(std::string const& id)
    {
      auto it = this->networks_status().find(id);
      if (it == this->networks_status().end())
        throw Exception{
            gap_error,
            "Unknown network id '" + id + "'",
        };
      return *(it->second);
    }

  }
}
