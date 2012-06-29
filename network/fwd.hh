#ifndef ELLE_NETWORK_FWD_HH
# define ELLE_NETWORK_FWD_HH

# include <elle/network/Tag.hh>

namespace elle
{
  namespace network
  {

    class AbstractSocket;
    class Header;
    class Host;
    class Locus;
    template <const Tag G>
    struct Message;
    class Parcel;
    template <const Tag I,
              const Tag O,
              const Tag E>
    class Procedure;
    template <const Character* C>
    struct Range;
    class TCPSocket;

  }
}

#endif
