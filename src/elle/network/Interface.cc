#include <bitset>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#elif defined INFINIT_ANDROID
# include <arpa/inet.h>
# include <elle/network/ifaddrs_android.h>
# include <net/if.h>
# include <netinet/in.h>
# include <sys/socket.h>
#else
# include <arpa/inet.h>
# include <ifaddrs.h>
# include <net/if.h>
# include <netinet/in.h>
# include <sys/socket.h>
#endif

#include <sys/types.h>

#ifdef AF_LINK // BSD systems
# include <net/if_dl.h>
#endif
#ifdef AF_PACKET // linux
# include <netpacket/packet.h>
#endif

#include <elle/Exception.hh>
#include <elle/printf.hh>
#include <elle/network/Interface.hh>
#include <elle/os/environ.hh>

#ifdef AF_PACKET
# define hw_addr_family AF_PACKET
# define hw_addr_type sockaddr_ll
# define hw_addr_len(addr) addr->sll_halen
# define hw_addr_data(addr) addr->sll_addr
#else
# define hw_addr_family AF_LINK
# define hw_addr_type sockaddr_dl
# define hw_addr_len(addr) (addr->sdl_alen)
# define hw_addr_data(addr) (addr->sdl_data + addr->sdl_nlen)
#endif

namespace elle
{
  namespace network
  {
#ifdef INFINIT_WINDOWS

    std::map<std::string, Interface>
    Interface::get_map(Filter filter)
    {
      static bool wsa_started = false;
      if (!wsa_started)
      {
        WSAData data;
        auto res = WSAStartup(MAKEWORD(2, 2), &data);
        if (res != 0)
        {
          int errnum = WSAGetLastError();
          throw elle::Exception{
            elle::sprintf("WSAStartup failed: %d", errnum)};
        }
        wsa_started = true;
      }

      auto addresses = std::map<std::string, Interface>{};
      char local_hostname[512] = "";

      struct hostent* host = gethostbyname(local_hostname);
      if (host == nullptr)
      {
        int errnum = WSAGetLastError();
        std::string error;
        switch (errnum)
        {
          case WSANOTINITIALISED:
            error = "WSAStartup was not called";
            break;
          case WSAENETDOWN:
            error = "network subsystem failed";
            break;
          case WSAHOST_NOT_FOUND:
            error = "host not found";
            break;
          default:
            error = elle::sprintf("unkown error: %s", errnum);
            break;
        }
        throw elle::Exception(elle::sprintf(
                                "gethostbyname failed: %s", error));
      }
      for (int i = 0; host->h_addr_list[i] != 0; i++)
      {
        struct in_addr addr;
        Interface if_;

        if_.mac_address = "";
        addr.s_addr = *(u_long*)host->h_addr_list[i];
        if_.ipv4_address.emplace_back(inet_ntoa(addr));
        addresses[elle::sprintf("%s-%d", host->h_name, i)] = if_;
      }

      return addresses;
    }
#else
    namespace
    {
      bool
      check_ipv4_autoip(struct sockaddr* sock)
      {
        // 169.254.0.0
        static const auto autoip_addr    = 10101001111111100000000000000000_bits;
        // 255.255.0.0
        static const auto autoip_netmask = 11111111111111110000000000000000_bits;

        // This check only make sense in ipv4.
        if (sock->sa_family != AF_INET)
          return false;

        auto sin = (struct sockaddr_in*) sock;
        auto addr_bits = std::bitset<32>(ntohl(sin->sin_addr.s_addr));

        return autoip_addr == (autoip_netmask & addr_bits);
      }

      bool
      check_awdl(std::string const& ifa_name)
      {
        return ifa_name.find("awdl") == 0;
      }

      /// Whether a link-scope ipv6 address.
      bool
      is_linklocal(struct sockaddr const* addr)
      {
        if (addr->sa_family == AF_INET6)
        {
          auto const& inet_addr = *reinterpret_cast<sockaddr_in6 const*>(addr);
          return IN6_IS_ADDR_LINKLOCAL(&inet_addr.sin6_addr);
        }
        else
          return false;
      }
    }

    std::map<std::string, Interface>
    Interface::get_map(Filter filter)
    {
      auto map = std::map<std::string, Interface>{};

      ifaddrs* ifap = nullptr;
      if (getifaddrs(&ifap) != 0)
        return map;

      if (os::getenv("ELLE_NO_LINKLOCAL", false))
        filter |= Filter::no_linklocal;

      for (ifaddrs* iter = ifap; iter != nullptr; iter = iter->ifa_next)
      {
        if (!iter->ifa_addr)
          continue;

        // Apply filters
        if (filter & Filter::no_loopback
            && bool(iter->ifa_flags & IFF_LOOPBACK))
          continue;

        if (filter & Filter::only_up
            && !(iter->ifa_flags & IFF_UP))
          continue;

        if ((filter & Filter::no_autoip)
            && check_ipv4_autoip(iter->ifa_addr))
          continue;

        if ((filter & Filter::no_awdl)
            && check_awdl(iter->ifa_name))
          continue;

        if ((filter & Filter::no_linklocal)
            && is_linklocal(iter->ifa_addr))
          continue;

        switch (iter->ifa_addr->sa_family)
        {
        case AF_INET:
          {
            auto inet_addr = reinterpret_cast<sockaddr_in*>(iter->ifa_addr);
            auto tab = reinterpret_cast<uint8_t*>(&inet_addr->sin_addr.s_addr);
            std::ostringstream oss;
            for (size_t i = 0; i < sizeof(inet_addr->sin_addr.s_addr); ++i)
            {
              if (i)
                oss << '.';
              oss << static_cast<unsigned int>(tab[i]);
            }
            map[iter->ifa_name].ipv4_address.push_back(oss.str());
          }
          break;
        case AF_INET6:
          {
            auto const& inet_addr = *reinterpret_cast<sockaddr_in6 const*>(iter->ifa_addr);
            auto const& sin6_addr = inet_addr.sin6_addr;
            auto const tab = reinterpret_cast<uint8_t const*>(&sin6_addr.s6_addr);
            std::ostringstream oss;
            for (size_t i = 0; i < sizeof(sin6_addr.s6_addr); i+=2)
              elle::fprintf(oss, "%s%02x%02x",
                            i ? ":" : "",
                            static_cast<unsigned int>(tab[i]),
                            static_cast<unsigned int>(tab[i+1]));
            map[iter->ifa_name].ipv6_address.push_back(oss.str());
          }
          break;
        case hw_addr_family:
          {
            auto hw_addr = reinterpret_cast<hw_addr_type*>(iter->ifa_addr);
            std::ostringstream oss;
            for (size_t i = 0; i < hw_addr_len(hw_addr); ++i)
            {
              if (i)
                oss << ':';
              elle::fprintf(oss, "%02x",
                            static_cast<unsigned int>((uint8_t)hw_addr_data(hw_addr)[i]));
            }
            map[iter->ifa_name].mac_address = oss.str();
          }
          break;
        }
      }
      freeifaddrs(ifap);
      return map;
    }
#endif

    std::ostream&
    operator <<(std::ostream& stream, Interface const& interface)
    {
      return stream << interface.ipv4_address;
    }
  }
}
