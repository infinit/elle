//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Slot.hxx
//
// created       julien quintard   [sat feb 20 18:28:29 2010]
// updated       julien quintard   [mon feb 22 14:37:26 2010]
//

#ifndef ELLE_NETWORK_SLOT_HXX
#define ELLE_NETWORK_SLOT_HXX

namespace elle
{
  namespace network
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method converts a set of values into a UDP packet and sends it
    /// to the given address.
    ///
    template <const Tag G, typename... T>
    Status		Slot::Send(const Address&		address,
				   const T&...			arguments)
    {
      Packet		packet;

      // assemble a message.
      //if (Message::Pack<G>(arguments..., packet) == StatusError)
      //escape("unable to pack the arguments");

      // push the datagram into the socket.
      if (this->socket->writeDatagram((char*)packet.contents,
				      packet.size,
				      address.host.location,
				      address.port) != packet.size)
	escape(this->socket->errorString().toStdString().c_str());

      leave();
    }

  }
}

#endif
