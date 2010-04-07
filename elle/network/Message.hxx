//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Message.hxx
//
// created       julien quintard   [thu mar  4 14:53:29 2010]
// updated       julien quintard   [wed mar 31 14:52:54 2010]
//

#ifndef ELLE_NETWORK_MESSAGE_HXX
#define ELLE_NETWORK_MESSAGE_HXX

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is specialized for the tag zero because zero has
    /// not negative counterpart meaning that a conversation with the
    /// zero tag would be incorrect.
    ///
    /// the following makes this case impossible to reach.
    ///
    template <>
    class Message<TagNone>
    {
    };

  }
}

#endif
