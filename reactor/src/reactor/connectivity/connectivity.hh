#ifndef REACTOR_CONNECTIVITY_CONNECTIVITY_HH
# define REACTOR_CONNECTIVITY_CONNECTIVITY_HH

# include <cstdint>
# include <string>

namespace reactor
{
  namespace connectivity
  {
    struct Result
    {
      Result(std::string res, uint16_t local_port);

      std::string host;
      uint16_t remote_port;
      uint16_t local_port;
    };

    Result
    tcp(std::string const& host, int port);

    Result
    udp(std::string const& host, int port);

    std::string
    nat(std::string const& host, int port);

    Result
    rdv_utp(std::string const& host, int port, int xorit = 0);

    Result
    utp(std::string const& host, int port, int xorit = 0);
  }
}

#endif
