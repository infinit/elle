#pragma once
#ifndef  PLASMA_META_CLIENT_HXX
# define PLASMA_META_CLIENT_HXX

namespace plasma
{
  namespace meta
  {

      template <class Container>
      NetworkConnectDeviceResponse
      Client::network_connect_device(std::string const& network_id,
                             std::string const& device_id,
                             Container const& local_ips)
      {
        adapter_type local_adapter;
        adapter_type public_adapter;

        for (auto &a: local_ips)
        {
          local_adapter.emplace_back(a.first, a.second);
        }

        return this->_network_connect_device(network_id,
                                             device_id,
                                             local_adapter,
                                             public_adapter);
      }

      template <class Container1, class Container2>
      NetworkConnectDeviceResponse
      Client::network_connect_device(std::string const& network_id,
                             std::string const& device_id,
                             Container1 const& local_ips,
                             Container2 const& public_endpoints)
      {
        adapter_type local_adapter;
        adapter_type public_adapter;

        for (auto &a: local_ips)
        {
          local_adapter.emplace_back(a.first, a.second);
        }

        for (auto &a: public_endpoints)
        {
          public_adapter.emplace_back(a.first, a.second);
        }

        return this->_network_connect_device(network_id,
                                             device_id,
                                             local_adapter,
                                             public_adapter);
      }
  }
}

#endif /* end of include guard:  PLASMA_META_CLIENT_HXX */
