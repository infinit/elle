#include <ostream>

#include <elle/reactor/network/proxy.hh>
#include <utility>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /*-----------.
      | Proxy Type |
      `-----------*/
      std::ostream&
      operator << (std::ostream& output, ProxyType type)
      {
        switch (type)
        {
          case ProxyType::None:
            return output << "None";
          case ProxyType::HTTP:
            return output << "HTTP";
          case ProxyType::HTTPS:
            return output << "HTTPS";
          case ProxyType::SOCKS:
            return output << "SOCKS";
          default:
            return output << "Unknown Proxy Type";
        }
      }

      /*------.
      | Proxy |
      `------*/
      Proxy::Proxy(ProxyType type,
                   std::string  host,
                   uint16_t port,
                   std::string  username,
                   std::string  password)
        : _type(type)
        , _host(std::move(host))
        , _port(port)
        , _username(std::move(username))
        , _password(std::move(password))
      {}

      Proxy::Proxy()
        : Self(ProxyType::None, "", 0, "", "")
      {}

      Proxy::~Proxy()
      = default;

      void
      Proxy::print(std::ostream& stream) const
      {
        stream << "Proxy(" << this->type() << ") "
               << this->host() << ":" << this->port();
      }
    }
  }
}
