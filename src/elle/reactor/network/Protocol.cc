#include <ostream>

#include <elle/reactor/network/Protocol.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      std::ostream&
      operator <<(std::ostream& out, Protocol p)
      {
        switch (p)
        {
          case Protocol::tcp:
            out << "reactor::network::Protocol::tcp";
            break;
          case Protocol::utp:
            out << "reactor::network::Protocol::utp";
            break;
          default:
            out << "unknow Protocol";
        }
        return out;
      }
    } /* network */
  } /* reactor */
}
