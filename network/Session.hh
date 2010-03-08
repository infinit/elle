//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Session.hh
//
// created       julien quintard   [thu feb 25 13:50:42 2010]
// updated       julien quintard   [fri mar  5 10:41:32 2010]
//

#ifndef ELLE_NETWORK_SESSION_HH
#define ELLE_NETWORK_SESSION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Misc.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

      // XXX
      // context =lie a= (socket[udp,local], address) | (socket[tcp])
      // session =lie= request/reply, call/return etc. SINGLETON
      //
      // Send(...);
      // Transmit(...);
      // Request(..., context);
      // Call(..., context);
      // XXX

      // XXX
      // Send(): asynchronous i.e send and return.
      // Transmit(): synchronous i.e send and wait for a ACK (unless in TCP)
      // Receive():
      // XXX

    ///
    /// XXX link user-specific data to a conversation
    ///
    /// XXX \todo this class should be put in the local storage so that
    ///     every thread has one.
    ///
    class Session
    {
    public:
      //
      // static methods
      //
      static Status	Open();
      static Status	Close();
    };

  }
}

#endif
