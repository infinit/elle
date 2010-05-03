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
// updated       julien quintard   [sun may  2 21:12:21 2010]
//

#ifndef ELLE_NETWORK_SLOT_HXX
#define ELLE_NETWORK_SLOT_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/network/Packet.hh>

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
      Natural64		offset;
      Natural64		point;

      enter();

      // create an data for the inputs.
      if (data.Create() == StatusError)
	escape("unable to create the data");

      // serialize the inputs.
      if (inputs.Serialize(data) == StatusError)
	escape("unable to serialize the inputs");

      // prepare the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // retrieve the offset---i.e size attribute---of the packet at this
      // time so that it can be used later to update parts of the archive,
      // especially the header.
      offset = packet.size;

      // serialize the the header though, at this point, it has not
      // been created.
      if (packet.Serialize(header) == StatusError)
	escape("unable to serialize the header");

      // save the offset just following the header's serialization.
      point = packet.size;

      // serialize the the data.
      if (packet.Serialize(data) == StatusError)
	escape("unable to serialize the data");

      // create the header now that we know that final archive's size.
      if (header.Create(event, inputs.tag, packet.size - point) == StatusError)
	escape("unable to create the header");

      // update the header.
      if (packet.Update(offset, header) == StatusError)
	escape("unable to update the header");

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
