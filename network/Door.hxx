//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Door.hxx
//
// created       julien quintard   [tue feb 23 13:44:55 2010]
// updated       julien quintard   [wed mar 10 20:07:31 2010]
//

#ifndef ELLE_NETWORK_DOOR_HXX
#define ELLE_NETWORK_DOOR_HXX

namespace elle
{
  namespace network
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method send a local packet in an asynchronous manner.
    ///
    /// XXX for now, just forward to transmit!
    ///
    template <typename I>
    Status		Door::Send(const I&			inputs,
				   const Identifier&		identifier)
    {
      return (this->Transmit(inputs, identifier));
    }

    ///
    /// this method 
    ///
    template <typename I>
    Status		Door::Transmit(const I&			inputs,
				       const Identifier&	identifier)
    {
      Packet		packet;
      Header		header;
      Data		data;

      enter();

      // create an data for the inputs.
      if (data.Create() == StatusError)
	escape("unable to create the data");

      // serialize the inputs.
      if (inputs.Serialize(data) == StatusError)
	escape("unable to serialize the inputs");

      // create the header.
      if (header.Create(identifier, inputs.tag, data.size) == StatusError)
	escape("unable to create the header");

      // prepare the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the message i.e the header followed by the data.
      if (packet.Serialize(header, data) == StatusError)
	escape("unable to serialize the message");

      // push the packet to the socket.
      if (this->socket->write((const char*)packet.contents,
			      packet.size) != packet.size)
	escape("unable to write the packet");

      // flush to start sending data immediately.
      this->socket->flush();

      leave();
    }

  }
}

#endif
