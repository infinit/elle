#ifndef REACTOR_NETWORK_PROXY_HH
# define REACTOR_NETWORK_PROXY_HH

# include <iosfwd>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

namespace reactor
{
  namespace network
  {
    /*-----------.
    | Proxy Type |
    `-----------*/
    /// This enum is based on that given by curl and OS X.
    /// It's unlikely that we'll use anything other than HTTP(S) and SOCKS5.
    enum class ProxyType
    {
      None,
      HTTP,
      HTTPS,
      SOCKS,
    };

    std::ostream&
    operator <<(std::ostream& output, ProxyType type);

    /*------.
    | Proxy |
    `------*/
    class Proxy:
      public elle::Printable
    {
    public:
      using Self = reactor::network::Proxy;

    public:
      Proxy(ProxyType type,
            std::string const& host,
            uint16_t port,
            std::string const& username = "",
            std::string const& password = "");

      /// Creates empty proxy of "None" type.
      Proxy();

      ~Proxy();

      ELLE_ATTRIBUTE_R(ProxyType, type);
      ELLE_ATTRIBUTE_R(std::string, host);
      ELLE_ATTRIBUTE_R(uint16_t, port);
      ELLE_ATTRIBUTE_R(std::string, username);
      ELLE_ATTRIBUTE_R(std::string, password);

    public:

      void
      print(std::ostream& stream) const override;
    };

    /// Compares only the type and host of given proxies.
    inline
    bool
    operator== (const Proxy& lhs, const Proxy& rhs)
    {
      if (lhs.type() == rhs.type() && lhs.host() == rhs.host())
        return true;
      else
        return false;
    }
  }
}

#endif
