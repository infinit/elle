#include <ostream>

#include <elle/reactor/network/proxy.hh>

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
                   std::string const& host,
                   uint16_t port,
                   std::string const& username,
                   std::string const& password)
        : _type(type)
        , _host(host)
        , _port(port)
        , _username(username)
        , _password(password)
      {}

      Proxy::Proxy()
        : Self(ProxyType::None, "", 0, "", "")
      {}

      Proxy::~Proxy()
      {}

      void
      Proxy::print(std::ostream& stream) const
      {
        stream << "Proxy(" << this->type() << ") "
               << this->host() << ":" << this->port();
      }
    }
  }
}
