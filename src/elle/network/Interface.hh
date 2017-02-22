#ifndef ELLE_NETWORK_INTERFACE_HH
# define ELLE_NETWORK_INTERFACE_HH

# include <map>
# include <string>
# include <vector>

# include <elle/compiler.hh>

namespace elle
{
  namespace network
  {
    /// Represents a network interface.
    struct ELLE_API Interface
    {
    public:
      std::string mac_address;
      std::string ipv4_address;
      std::vector<std::string> ipv6_address;

    public:
      enum class Filter : int
      {
        none        = 0x00,
        no_loopback = 0x01, /// Exclude loopback interface.
        only_up     = 0x02, /// Exclude down interfaces.
        no_autoip   = 0x04, /// Exclude autoip interfaces.
        no_awdl     = 0x08, /// Exclude Apple Wireless Direct Link interfaces.
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
    operator &(Interface::Filter const lhs,
               Interface::Filter const rhs);

    // Mix two filters.
    inline
    Interface::Filter
    operator |(Interface::Filter const lhs,
               Interface::Filter const rhs);
  }
}

# include <elle/network/Interface.hxx>

#endif
