#pragma once

#include <iosfwd>

#include <elle/attribute.hh>
#include <elle/Printable.hh>

namespace elle
{
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
      /// Binding to a Proxy.
      class Proxy:
        public elle::Printable
      {
      public:
        using Self = elle::reactor::network::Proxy;

      public:
        /// Construct a Proxy.
        ///
        /// \param type The Proxy type.
        /// \param host The host name of the proxy.
        /// \param port The port used by the proxy.
        /// \param username The username on the proxy.
        /// \param password The password on the proxy.
        Proxy(ProxyType type,
              std::string  host,
              uint16_t port,
              std::string  username = "",
              std::string  password = "");

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
}
