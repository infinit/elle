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
// updated       julien quintard   [tue mar 23 21:08:48 2010]
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
    /// to the given address in an asynchronous way meaning that the sender
    /// does not wait for an acknowledgment for continuing.
    ///
    template <typename I>
    Status		Slot::Send(const Address&		address,
				   const I&			inputs,
				   const Event&			event)
    {
      Packet		packet;
      Header		header;
      Data		data;

      enter();

      // create the data.
      if (data.Create() == StatusError)
	escape("unable to create the data");

      // serialize the inputs
      if (inputs.Serialize(data) == StatusError)
	escape("unable to serialize the inputs");

      // create the header.
      if (header.Create(event, inputs.tag, data.size) == StatusError)
	escape("unable to create the header");

      // prepare the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the message i.e the header followed by the parameters.
      if (packet.Serialize(header, data) == StatusError)
	escape("unable to serialize the message");

      printf("[XXX] Slot::Send(%u)\n", packet.size);

      // push the datagram into the socket.
      if (this->socket->writeDatagram((char*)packet.contents,
				      packet.size,
				      address.host.location,
				      address.port) != packet.size)
	escape(this->socket->errorString().toStdString().c_str());

      leave();
    }

    ///
    /// XXX synchronous (wait until received before returning.
    ///
    template <typename I>
    Status		Slot::Transmit(const Address&		address,
				       const I&			inputs,
				       const Event&		event)
    {
      enter();

      // XXX

      leave();
    }

  }
}

#endif
