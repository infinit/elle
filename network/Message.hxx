//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/network/Message.hxx
//
// created       julien quintard   [thu mar  4 14:53:29 2010]
// updated       julien quintard   [mon apr 26 18:27:56 2010]
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
    class Message<TagNone>:
      public Meta
    {
    };

  }
}

#endif
