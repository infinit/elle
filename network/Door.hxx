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
// updated       julien quintard   [sun mar  7 21:59:42 2010]
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

      printf("Door::Tramsit()ted...\n");

      leave();
    }

    ///
    /// XXX return packet for the given identifier (blocking).
    ///
    template <typename O>
    Status		Door::Receive(const Identifier&		identifier,
				      O				outputs)
    {
      Packet		packet;

      enter();

      printf("Door::Receive()...\n");

      // wait for data.
      if (this->socket->waitForReadyRead() == false)
	escape("unable to read data");

      /// \todo XXX the messages with the wrong identifier should be
      ///           bufferised.

      printf("Door::Receive()...reading\n");

      // read the next packet.
      if (this->Read(packet) == StatusError)
	escape("unable to read the next packet");

      printf("Door::Receive()...read\n");

      // extract the outputs arguments.
      if (outputs.Extract(packet) == StatusError)
	escape("unable to extract the output arguments");

      leave();
    }

    /* XXX
    ///
    /// XXX send(asynchronous) + receive(blocking).
    ///
    /// XXX fournir une interface asynchrone genre tu passes des arguments
    /// outputs et tu recois un ticket. et plus tard tu peux interroger le
    /// ticket pour savoir si c'est bon.
    ///
    template <typename I,
	      typename O>
    Status		Request(const Address&			address,
				const I&			inputs,
				O&				ouputs)
    {
      enter();

      // XXX

      leave();
    }
    */

    ///
    /// XXX transmit(synchro) + receive(blocking)
    ///
    template <typename I,
	      typename O>
    Status		Door::Call(const I&			inputs,
				   O				outputs)
    {
      Identifier	identifier;

      enter();

      // generate a random identifier.
      if (identifier.Generate() == StatusError)
	escape("unable to generate a random identifier");

      // transmit the input message. // XXX identifier
      if (this->Transmit(inputs, identifier) == StatusError)
	escape("unable to transmit the message");

      // wait for the response.
      if (this->Receive(identifier, outputs) == StatusError)
	escape("unable to receive the response");

      leave();
    }

    ///
    /// XXX take current session's id and correspondent's address and send it.
    ///
    template <typename I>
    Status		Door::Reply(const I&			inputs)
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX idem
    ///
    template <typename I>
    Status		Door::Return(const I&			inputs)
    {
      enter();

      // XXX

      leave();
    }

  }
}

#endif
