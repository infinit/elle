#ifndef ELLE_NETWORK_PACKET_HH
# define ELLE_NETWORK_PACKET_HH

# include <elle/utility/Buffer.hh>

namespace elle
{
  namespace network
  {

    /// this type represents a message in its network format i.e an archive.
    typedef elle::utility::Buffer             Packet;

  }
}

#endif
