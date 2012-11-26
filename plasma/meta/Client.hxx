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
                             Container const& local_ips,
                             std::string const* external_ip,
                             uint16_t external_port)
      {
        std::vector<std::pair<std::string, uint16_t>> adapter;

        for (auto &a: local_ips)
        {
          adapter.emplace_back(a.first, a.second);
        }

        /* 
         * This adapter is only there in order for the Json and Serialization
         * classes to be forward declared
         */
        this->_network_connect_device(network_id,
                                      device_id,
                                      adapter,
                                      external_ip,
                                      external_port);
      }
  }
}

#endif /* end of include guard:  PLASMA_META_CLIENT_HXX */
