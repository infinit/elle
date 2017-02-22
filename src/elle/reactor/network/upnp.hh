#pragma once

#include <string>
#include <memory>

#include <elle/reactor/network/Protocol.hh>
#include <elle/reactor/mutex.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      class UPNP;
      /** Handle for one port redirection provided by a router.
      * Moveable, non-copyable. Mapping gets released on destruction.
      */
      class PortMapping
      {
      public:
        PortMapping();
        PortMapping(PortMapping const&) = delete;
        void operator=(PortMapping const&) = delete;
        PortMapping(PortMapping && b);
        void operator=(PortMapping&& b);
        ~PortMapping();
        /// Release redirection handle. The redirection will never be
        /// unregistered
        void release();
        operator bool () const;
        std::string internal_host;
        std::string internal_port;
        std::string external_host;
        std::string external_port;
        Protocol    protocol;
        std::shared_ptr<UPNP> _owner;
      };

      std::ostream& operator << (std::ostream&, PortMapping const&);

      class UPNPImpl;

      /** UPNP-IGD client, used to query public IP address and make port
       * redirection requests to a router.
       */
      class UPNP: public std::enable_shared_from_this<UPNP>
      {
      public:
        static
        std::shared_ptr<UPNP>
        make();
        ~UPNP();
        /// Must be called only once. Throw if no suitable IGD server is found.
        void
        initialize();
        /// Return external_ip as returned by the router, or throw.
        std::string
        external_ip();
        /// Setup a port redirection for this host.
        PortMapping
        setup_redirect(Protocol p,
                       unsigned short port);

        /// Whether a suitable router could be contacted through UPNP.
        ELLE_ATTRIBUTE_R(bool, available);

      private:
        class PrivateGuard{};
      public:
        UPNP(PrivateGuard);
      private:

        void release(PortMapping &);
        void _release(char const* port, char const* protocol);
        friend class PortMapping;
        std::shared_ptr<UPNPImpl> _impl;
        Mutex     _mutex;
      };
    }
  }
}
