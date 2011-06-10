//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Packet.hh
//
// created       julien quintard   [mon feb 22 11:21:39 2010]
// updated       julien quintard   [sun mar  7 12:39:53 2010]
//

#ifndef ELLE_NETWORK_PACKET_HH
#define ELLE_NETWORK_PACKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/package/Archive.hh>

namespace elle
{
  using namespace package;

  namespace network
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this type represents a message in its network format i.e an archive.
    ///
    typedef Archive		Packet;

  }
}

#endif
