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
// updated       julien quintard   [sun mar 21 21:41:50 2010]
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
    Status		Door::Send(const I			inputs,
				   const Identifier&		identifier)
    {
      return (this->Transmit(inputs, identifier));
    }

    ///
    /// XXX
    ///
    template <typename I>
    Status		Door::Transmit(const I			inputs,
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

      // XXX
      printf("[XXX] Door::Transmitting(%qu)\n", identifier.value);

      // push the packet to the socket.
      if (this->socket->write((const char*)packet.contents,
			      packet.size) != packet.size)
	escape("unable to write the packet");

      // flush to start sending data immediately.
      this->socket->flush();

      leave();
    }

    ///
    /// XXX
    ///
    template <typename I,
	      typename O>
    Status		Door::Call(const I			inputs,
				   O				outputs)
    {
      Identifier	identifier;

      enter();

      // generate an identifier to link the request with the response.
      if (identifier.Generate() == StatusError)
	escape("unable to generate the identifier");

      // transmit the inputs.
      if (this->Transmit(inputs, identifier) == StatusError)
	escape("unable to transmit the inputs");

      // wait for the reply.
      if (this->Receive(identifier, outputs) == StatusError)
	escape("unable to transmit the inputs");

      leave();
    }

    ///
    /// XXX
    ///
    template <typename O>
    Status		Door::Receive(const Identifier&		identifier,
				      O				outputs)
    {
      enter();

      // ask the network to block until the message with the specified
      // identifier is received.
      if (Network::Receive(identifier, outputs) == StatusError)
	escape("unable to receive the specified message");

      leave();
    }

    ///
    /// XXX
    ///
    template <typename I>
    Status		Door::Reply(const I			inputs)
    {
      enter();

      // transmit a message as a response by using the identifier of
      // the received message i.e the current session.
      if (this->Transmit(inputs, session->identifier) == StatusError)
	escape("unable to transmit the reply");

      leave();
    }

  }
}

#endif
