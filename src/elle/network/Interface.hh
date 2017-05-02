#pragma once

#include <map>
#include <string>
#include <vector>

#include <elle/compiler.hh>

namespace elle
{
  namespace network
  {
    /// Represents a network interface.
    struct ELLE_API Interface
    {
    public:
      std::string mac_address;
      std::vector<std::string> ipv4_address;
      std::vector<std::string> ipv6_address;

    public:
      enum class Filter : int
      {
        none         = 0x00,
        no_loopback  = 1<<1, /// Exclude loopback interface.
        only_up      = 1<<2, /// Exclude down interfaces.
        no_autoip    = 1<<3, /// Exclude autoip interfaces.
        no_awdl      = 1<<4, /// Exclude Apple Wireless Direct Link interfaces.
        no_linklocal = 1<<5, /// Exclude scope-link addresses.
      };
    public:
      /// Retreive a map of local interfaces, indexed by interface name.
      /// You can filter results with using any combination of Filters.
      static
      std::map<std::string, Interface>
      get_map(Filter filter = Filter::none);
    };

    ELLE_API
    std::ostream&
    operator <<(std::ostream& stream, Interface const& interface);

    // Check whether or not a flag is enabled.
    inline
    bool
    operator &(Interface::Filter lhs, Interface::Filter rhs);

    // Mix two filters.
    inline
    Interface::Filter
    operator |(Interface::Filter lhs, Interface::Filter rhs);

    // Augment a filter.
    inline
    Interface::Filter&
    operator |=(Interface::Filter& lhs, Interface::Filter rhs);
  }
}

#include <elle/network/Interface.hxx>
