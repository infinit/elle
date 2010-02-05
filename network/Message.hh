//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Message.hh
//
// created       julien quintard   [wed feb  3 22:01:47 2010]
// updated       julien quintard   [thu feb  4 00:01:10 2010]
//

#ifndef ELLE_NETWORK_MESSAGE_HH
#define ELLE_NETWORK_MESSAGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/archive/Archive.hh>

#include <elle/network/Tag.hh>

namespace elle
{
  using namespace io;
  using namespace archive;

  namespace network
  {

    ///
    /// this class abstracts messages so that any message intended to be
    /// sent on the network, through a socket, should inherit this class.
    ///
    class Message:
      public Dumpable, public Archivable
    {
      // nothing
    };

  }
}

#endif
