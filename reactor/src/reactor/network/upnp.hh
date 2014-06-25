#ifndef REACTOR_NETWORK_UPNP_HH
# define REACTOR_NETWORK_UPNP_HH

#include <string>
#include <reactor/network/Protocol.hh>
#include <reactor/mutex.hh>

namespace reactor
{
  namespace network
  {
    class UPNP;
    /** Handle for one port redirection provided by a router.
    * Moveable, non-copyable. Redirection gets released on destruction.
    */
    class PortRedirection
    {
    public:
      PortRedirection();
      PortRedirection(PortRedirection const&) = delete;
      void operator=(PortRedirection const&) = delete;
      PortRedirection(PortRedirection && b);
      void operator=(PortRedirection&& b);
      ~PortRedirection();
      /// Release redirection handle. The redirection will never be unregistered
      void release();
      std::string internal_host;
      std::string internal_port;
      std::string external_host;
      std::string external_port;
      Protocol    protocol;
      UPNP* _owner;
    };

    std::ostream& operator << (std::ostream&, PortRedirection const&);

    class UPNPImpl;

    /** UPNP-IGD client, used to query public IP address and make port
     * redirection requests to a router.
     */
    class UPNP
    {
    public:
      UPNP();
      ~UPNP();
      /// Must be called only once. Throw if no suitable IGD server is found.
      void initialize();
      std::string external_ip();
      /// Setup a port redirection for this host.
      PortRedirection setup_redirect(Protocol p,
                                     unsigned short port);
      /// Whether a suitable router could be contacted through UPNP.
      ELLE_ATTRIBUTE(bool, available);
    private:
      void _setup_redirect(Protocol p, unsigned short port, PortRedirection& res);
      void _initialize();
      void release(PortRedirection &);
      void _release(PortRedirection&);
      friend class PortRedirection;
      UPNPImpl* _impl;
      Mutex     _mutex;
    };
  }
}

#endif