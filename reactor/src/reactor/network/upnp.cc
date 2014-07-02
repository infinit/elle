#include <reactor/network/upnp.hh>
#include <reactor/scheduler.hh>

#include <elle/log.hh>
#include <miniupnpc.h>
#include <upnpcommands.h>
#include <upnperrors.h>

ELLE_LOG_COMPONENT("reactor.network.upnp");

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
      struct UPNPDev * devlist;
      struct UPNPUrls urls;
      struct IGDdatas data;
      char lanaddr[64];
      boost::optional<std::string> externalIPAddress;
      int valid_igd;
    };

    UPNP::UPNP(PrivateGuard)
    : _available(false)
    , _impl(0)
    {}

    std::shared_ptr<UPNP>
    UPNP::make()
    {
      return std::make_shared<UPNP>(PrivateGuard{});
    }

    UPNP::~UPNP()
    {
      if (_impl)
      {
        FreeUPNPUrls(&_impl->urls);
        freeUPNPDevlist(_impl->devlist);
      }
      delete _impl;
    }

    void
    UPNP::initialize()
    {
      reactor::Lock lock(_mutex);
      if (_impl)
        throw elle::Exception("UPNP instance already initialized");
      _impl = new UPNPImpl();
      reactor::background(std::bind(&UPNP::_initialize, this));
      ELLE_TRACE("UPNP initialized: lan=%s, igd=%s, external=%s",
                 _impl->lanaddr,
                 _impl->valid_igd,
                 _impl->externalIPAddress?
                   *_impl->externalIPAddress
                   : std::string("unknown"));
      _available = true;
    }

    void
    UPNP::_initialize()
    { // THREADED
      // timeoutMS multicastiface, daemonpath, sameport, use_ipv6
      int error = 0;
      _impl->devlist = upnpDiscover(2000, 0, 0, 0, 0, &error);
      if (!_impl->devlist)
      {
        ELLE_TRACE("upnpDiscover failed with %s", error);
        throw std::runtime_error(
          elle::sprintf("UPNP device discovery failure: %s", error));
      }
      _impl->valid_igd = UPNP_GetValidIGD(_impl->devlist, &_impl->urls,
                                          &_impl->data,
                                          _impl->lanaddr, sizeof(_impl->lanaddr));
      ELLE_TRACE("GetValidIGD: %s", _impl->valid_igd);
      /*
       1 ok urls.constrolURL
       2 not consnnected igd
       3 some device found
       0 no upnp device found
      */
      // be optimistic, and try to go on on !0
      if (!_impl->valid_igd)
        throw std::runtime_error("Could not find any IGD device");
      char externalIPAddress[40];
      int r = UPNP_GetExternalIPAddress(_impl->urls.controlURL,
                                        _impl->data.first.servicetype,
                                        externalIPAddress);
      if (r == UPNPCOMMAND_SUCCESS)
        _impl->externalIPAddress = externalIPAddress;
    }

    PortMapping
    UPNP::setup_redirect(Protocol p, unsigned short port)
    {
      reactor::Lock lock(_mutex);
      if (!_impl)
        throw elle::Exception("UPNP not initialized");
      PortMapping result;
      reactor::background(
        std::bind(&UPNP::_setup_redirect, this, p, port, std::ref(result)));
      return result;
    }

    void
    UPNP::_setup_redirect(Protocol p, unsigned short port, PortMapping& res)
    { // THREADED
      char effectivePort[6];
      std::string port_string = boost::lexical_cast<std::string>(port);
      ELLE_TRACE("AddAnyPortMapping(%s, %s, %s)",
                 port_string, port_string, _impl->lanaddr);
      int r = UPNP_AddAnyPortMapping(
        _impl->urls.controlURL,
        _impl->data.first.servicetype,
        port_string.c_str(), port_string.c_str(),
        _impl->lanaddr,
        "reactor",
        p == Protocol::tcp? "TCP" : "UDP",
        0, // remotehost
        "0", effectivePort);
      if (r != UPNPCOMMAND_SUCCESS)
      {
        ELLE_TRACE("AddAnyPortMapping failed with %s", strupnperror(r));
        // Fallback to AddPortMapping (AddAnyPortMapping seems to only be
        // implemented by some IGDs).
        r = UPNP_AddPortMapping(
          _impl->urls.controlURL,
          _impl->data.first.servicetype,
           port_string.c_str(), port_string.c_str(),
           _impl->lanaddr,
           "reactor",
           p == Protocol::tcp? "TCP" : "UDP",
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
        _impl->urls.controlURL,
        _impl->data.first.servicetype,
        effectivePort,
        p == Protocol::tcp? "TCP" : "UDP", NULL/*remoteHost*/,
        intClient, intPort,
        nullptr/*desc*/,
        nullptr/*enabled*/,
        duration);
      if(r!=UPNPCOMMAND_SUCCESS)
        throw std::runtime_error(
          elle::sprintf("GetSpecificPortMappingEntry: %s", strupnperror(r)));
      res._owner = shared_from_this();
      res.protocol = p;
      res.internal_host = _impl->lanaddr;
      res.internal_port = intPort;
      if (_impl->externalIPAddress)
        res.external_host = *_impl->externalIPAddress;
      res.external_port = effectivePort;
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
    UPNP::release(PortMapping & redirect)
    {
      reactor::Lock lock(_mutex);
      if (!_impl)
        throw elle::Exception("UPNP not initialized");
      reactor::background(std::bind(&UPNP::_release, this, std::ref(redirect)));
    }

    void
    UPNP::_release(PortMapping & redirect)
    {
      ELLE_TRACE("Releasing redirection %s", redirect);
      int r = UPNP_DeletePortMapping(_impl->urls.controlURL,
                                     _impl->data.first.servicetype,
                                     redirect.external_port.c_str(),
                                     redirect.protocol == Protocol::tcp?"TCP":"UDP",
                                     0);
      ELLE_TRACE("Mapping %s released with %s", redirect, strupnperror(r));
    }

    PortMapping::PortMapping()
    : _owner(0)
    {
    }

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