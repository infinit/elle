//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed feb  3 22:01:47 2010]
//

#ifndef ELLE_NETWORK_MESSAGE_HH
#define ELLE_NETWORK_MESSAGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Tag.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a message which is composed of a header and
    /// a set of parameters.
    ///
    /// it is important to notice that the Message class does not implement
    /// the Archivable interface because, whenever a message is received,
    /// the network sub-system first fetches the header in order to verify
    /// the message validity but also in order to extract the tag.
    ///
    /// then, the parameters can be retrieved properly. indeed, without the
    /// tag, the system cannot know the number or types of the parameters.
    /// that is why messages are split into two different logical parts.
    ///
    template <const Tag G>
    struct Message
    {
      // nothing
    };

  }
}

#endif
