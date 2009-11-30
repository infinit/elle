//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Packet.hh
//
// created       julien quintard   [fri nov 27 19:14:25 2009]
// updated       julien quintard   [mon nov 30 13:00:01 2009]
//

#ifndef ELLE_NETWORK_PACKET_HH
#define ELLE_NETWORK_PACKET_HH

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

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts the notion of packet transmitted over the network.
    ///
    /// the tag is used to differenciate the multiple packet types.
    ///
    /// note that this tag must not be serialized in any circumstances as
    /// it is the role of the Send()/Receive() methods to deal with it.
    ///
    class Packet:
      public Dumpable, public Archivable
    {
    public:
      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Tag		tag;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Packet.hxx>

#endif
