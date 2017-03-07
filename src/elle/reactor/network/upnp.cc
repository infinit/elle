#include <miniupnpc.h>
#include <upnpcommands.h>
#include <upnperrors.h>

#include <elle/log.hh>
#include <elle/reactor/network/upnp.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/utility/Move.hh>

ELLE_LOG_COMPONENT("elle.reactor.network.upnp");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /* Thread model:
       * backend operations are threaded through reactor::background.
       * invocation is reactor::Mutex protected, so only 1 can run in parallel
       *
       */
      class UPNPImpl
      {
      public:
        UPNPImpl()
        {
          memset(&urls, 0, sizeof(UPNPUrls));
        }

        ~UPNPImpl()
        {
          FreeUPNPUrls(&this->urls);
          if (this->devlist)
            freeUPNPDevlist(this->devlist);
        }

        UPNPDev* devlist{0};
        void _initialize();
        void _setup_redirect(Protocol p,
                             unsigned short port,
                             PortMapping& res,
                             std::shared_ptr<UPNP> owner);
        struct UPNPUrls urls;
        struct IGDdatas data;
        char lanaddr[64];
        boost::optional<std::string> externalIPAddress;
        int valid_igd;
      };

      UPNP::UPNP(PrivateGuard)
        : _available(false)
        , _impl()
      {}

      std::shared_ptr<UPNP>
      UPNP::make()
      {
        return std::make_shared<UPNP>(PrivateGuard{});
      }

      UPNP::~UPNP()
      = default;

      void
      UPNP::initialize()
      {
        ELLE_TRACE_SCOPE("%s: initialize", *this);
        reactor::Lock lock(_mutex);
        if (_impl)
          throw elle::Exception("UPNP instance already initialized");
        auto impl = std::make_shared<UPNPImpl>();
        this->_impl = impl;
        reactor::background([impl] { impl->_initialize(); });
        ELLE_DEBUG("%s: initialized: lan=%s, igd=%s, external=%s",
                   *this,
                   _impl->lanaddr,
                   _impl->valid_igd,
                   _impl->externalIPAddress ?
                   *_impl->externalIPAddress :
                   std::string("unknown"));
        this->_available = true;
      }

      void
      UPNPImpl::_initialize()
      { // THREADED
        // timeoutMS multicastiface, daemonpath, sameport, use_ipv6
        int error = 0;
        this->devlist = upnpDiscover(2000, nullptr, nullptr, 0, 0, &error);
        if (!this->devlist)
        {
          ELLE_TRACE("upnpDiscover failed with %s", error);
          throw std::runtime_error(
            elle::sprintf("UPNP device discovery failure: %s", error));
        }
        this->valid_igd = UPNP_GetValidIGD(this->devlist, &this->urls,
                                           &this->data,
                                           this->lanaddr, sizeof(this->lanaddr));
        ELLE_TRACE("GetValidIGD: %s", this->valid_igd);
        /*
          1 ok urls.constrolURL
          2 not consnnected igd
          3 some device found
          0 no upnp device found
        */
        // be optimistic, and try to go on on !0
        if (!this->valid_igd)
          throw std::runtime_error("Could not find any IGD device");
        char externalIPAddress[40];
        int r = UPNP_GetExternalIPAddress(this->urls.controlURL,
                                          this->data.first.servicetype,
                                          externalIPAddress);
        if (r == UPNPCOMMAND_SUCCESS)
          this->externalIPAddress = externalIPAddress;
      }

      PortMapping
      UPNP::setup_redirect(Protocol p, unsigned short port)
      {
        ELLE_TRACE_SCOPE("%s: setup redirect on %s (%s)", *this, port, p);
        reactor::Lock lock(_mutex);
        if (!_impl)
          throw elle::Exception("UPNP not initialized");
        auto impl = this->_impl;
        auto owner = this->shared_from_this();
        auto result = std::make_shared<PortMapping>();
        reactor::background(
          [impl, owner, result, p, port]
          {
            impl->_setup_redirect(p, port, *result, owner);
          });
        return std::move(*result);
      }

      // Threaded
      void
      UPNPImpl::_setup_redirect(Protocol p,
                                unsigned short port,
                                PortMapping& res,
                                std::shared_ptr<UPNP> owner)
      {
        char effectivePort[6];
        std::string port_string = std::to_string(port);
        std::string protocol_string = p == Protocol::tcp? "TCP" : "UDP";
        ELLE_TRACE("AddAnyPortMapping(%s, %s, %s, %s)",
                   protocol_string,
                   port_string, port_string, this->lanaddr);
        int r = UPNP_AddAnyPortMapping(
          this->urls.controlURL,
          this->data.first.servicetype,
          port_string.c_str(), port_string.c_str(),
          this->lanaddr,
          "reactor",
          protocol_string.c_str(),
          nullptr, // remotehost
          "0", effectivePort);
        if (r != UPNPCOMMAND_SUCCESS)
        {
          ELLE_TRACE("AddAnyPortMapping failed with %s", strupnperror(r));
          // Fallback to AddPortMapping (AddAnyPortMapping seems to only be
          // implemented by some IGDs).
          r = UPNP_AddPortMapping(
            this->urls.controlURL,
            this->data.first.servicetype,
            port_string.c_str(), port_string.c_str(),
            this->lanaddr,
            "reactor",
            protocol_string.c_str(),
            nullptr, // remotehost
            "0");
          strcpy(effectivePort, port_string.c_str()); // REALY?
        }
        if (r != UPNPCOMMAND_SUCCESS)
          throw std::runtime_error(
            elle::sprintf("UPNP_AddPortMapping: %s", strupnperror(r)));
        ELLE_TRACE("Add(Any?)PortMapping succeeded");
        // Validating that it worked by querying our entry.
        char intClient[40];
        char intPort[6];
        char duration[16];
        r = UPNP_GetSpecificPortMappingEntry(
          this->urls.controlURL,
          this->data.first.servicetype,
          effectivePort,
          protocol_string.c_str(), nullptr/*remoteHost*/,
          intClient, intPort,
          nullptr/*desc*/,
          nullptr/*enabled*/,
          duration);
        if(r!=UPNPCOMMAND_SUCCESS)
          throw std::runtime_error(
            elle::sprintf("GetSpecificPortMappingEntry: %s", strupnperror(r)));
        res._owner = owner;
        res.protocol = p;
        res.internal_host = this->lanaddr;
        res.internal_port = intPort;
        if (this->externalIPAddress)
          res.external_host = *this->externalIPAddress;
        res.external_port = effectivePort;
        ELLE_TRACE("_setup_redirect succeeded, returning %s", res);
      }

      std::string
      UPNP::external_ip()
      {
        reactor::Lock lock(_mutex);
        if (!_impl)
          throw elle::Exception("UPNP not initialized");
        if (!_impl->externalIPAddress)
          throw elle::Exception("Could not obtain external IP");
        return *_impl->externalIPAddress;
      }

      void
      UPNP::release(PortMapping& redirect)
      {
        ELLE_TRACE_SCOPE("%s: release %s", *this, redirect);
        if (!this->_impl)
          throw elle::Exception("UPNP not initialized");
        auto port = redirect.external_port.c_str();
        auto protocol = redirect.protocol == Protocol::tcp ? "TCP" : "UDP";
        if (reactor::Scheduler::scheduler())
        {
          reactor::Lock lock(_mutex);
          reactor::background([this, port, protocol] {
              this->_release(port, protocol);
            });
        }
        else
          this->_release(port, protocol);
      }

      void
      UPNP::_release(char const* port, char const* protocol)
      {
        ELLE_TRACE("%s: release %s", *this, port);
        int r = UPNP_DeletePortMapping(_impl->urls.controlURL,
                                       _impl->data.first.servicetype,
                                       port,
                                       protocol,
                                       nullptr);
        ELLE_DEBUG("%s: released with %s", *this, strupnperror(r));
      }

      PortMapping::PortMapping()
        : _owner(nullptr)
      {}

      PortMapping::~PortMapping()
      {
        if (_owner)
          _owner->release(*this);
      }

      void
      PortMapping::release()
      {
        if (!_owner)
          return;
        _owner->release(*this);
        _owner.reset();
      }

      PortMapping::PortMapping(PortMapping&& b)
      {
        *this = std::move(b);
      }

      void
      PortMapping::operator=(PortMapping&& b)
      {
        _owner = b._owner;
        b._owner.reset();
        protocol = b.protocol;
        std::swap(internal_host, b.internal_host);
        std::swap(internal_port, b.internal_port);
        std::swap(external_host, b.external_host);
        std::swap(external_port, b.external_port);
      }

      PortMapping::operator bool () const
      {
        return !!_owner;
      }

      std::ostream&
      operator << (std::ostream& os, PortMapping const& pr)
      {
        return os << (pr.protocol == Protocol::tcp? "TCP(" : "UDP(")
                  << pr.external_host << ':' << pr.external_port
                  << " -> "
                  << pr.internal_host << ':' << pr.internal_port
                  << ')';
      }
    }
  }
}
