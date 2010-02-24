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
// updated       julien quintard   [wed feb 24 08:49:31 2010]
//

#ifndef ELLE_NETWORK_SLOT_HXX
#define ELLE_NETWORK_SLOT_HXX

namespace elle
{
  namespace network
  {

//
// ---------- templates -------------------------------------------------------
//

    ///
    /// this method converts a set of values into a UDP packet and sends it
    /// to the given address.
    ///
    template <typename A>
    Status		Slot::Send(const Address&		address,
				   const A&			arguments)
    {
      Packet		packet;
      Header		header;
      Data		data;

      // create the data.
      if (data.Create() == StatusError)
	escape("unable to create the data");

      // serialize the arguments.
      if (arguments.Serialize(data) == StatusError)
	escape("unable to serialize the arguments");

      // create the header.
      if (header.Create(arguments.tag, data.size) == StatusError)
	escape("unable to create the header");

      // prepare the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the message i.e the header followed by the parameters.
      if (packet.Serialize(header, data) == StatusError)
	escape("unable to serialize the message");

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
