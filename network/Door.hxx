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
// updated       julien quintard   [wed apr 14 11:51:15 2010]
//

#ifndef ELLE_NETWORK_DOOR_HXX
#define ELLE_NETWORK_DOOR_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Outputs.hh>

#include <elle/Manifest.hh>

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
				   const Event&			event)
    {
      return (this->Transmit(inputs, event));
    }

    ///
    /// XXX
    ///
    template <typename I>
    Status		Door::Transmit(const I			inputs,
				       const Event&		event)
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
      if (header.Create(event, inputs.tag, data.size) == StatusError)
	escape("unable to create the header");

      // prepare the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the message i.e the header followed by the data.
      if (packet.Serialize(header, data) == StatusError)
	escape("unable to serialize the message");

      //printf("[XXX] Door::Transmit(tag[%u] identifier[%qu])\n",
      //header.tag, header.event.identifier);

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
      Event		event;

      enter();

      // generate an event to link the request with the response.
      if (event.Generate() == StatusError)
	escape("unable to generate the event");

      // transmit the inputs.
      if (this->Transmit(inputs, event) == StatusError)
	escape("unable to transmit the inputs");

      // wait for the reply.
      if (this->Receive(event, outputs) == StatusError)
	escape("unable to receive the outputs");

      leave();
    }

    ///
    /// XXX
    ///
    /// note that the session is never assigned when receive is used
    /// to manually receive messages. it is therefore the caller's
    /// responsability to keep information regarding who it is communicating
    /// with and using which socket.
    ///
    template <typename O>
    Status		Door::Receive(const Event&		event,
				      O				outputs)
    {
      Report		report;
      Parcel*		parcel;

      enter(instance(parcel));

      // process the call depending on the mode.
      if (this->mode == Socket::ModeAsynchronous)
	{
	  // block the current fiber until the given event is received.
	  if (Fiber::Wait(event, parcel) == StatusError)
	    escape("an error occured while waiting for a specific event");
	}
      else
	{
	  // synchronously read data from the socket i.e block if not
	  // available meaning that no even can be processed until
	  // the data is received.

	  // wait for the ready signal.
	  if (this->socket->waitForReadyRead() == false)
	    escape("an error occured while waiting for the ready signal");

	  // then read the parcel.
	  if (this->Read(parcel) != StatusTrue)
	    escape("unable to read a complete parcel from the door");
	}

      // check the tag.
      if (parcel->header->tag != outputs.tag)
	{
	  // test if the message received is an error, if so, append
	  // the report to the local one.
	  if (parcel->header->tag == TagError)
	    {
	      // extract the error message.
	      if (report.Extract(*parcel->data) == StatusError)
		escape("unable to extract the error message");

	      // report the remote error.
	      report(Report::TypeError, report);
	    }

	  escape("received a packet with an unexpected tag");
	}

      // extract the arguments.
      if (outputs.Extract(*parcel->data) == StatusError)
	escape("unable to extract the arguments");

      // delete the parcel.
      delete parcel;

      // stop tracking the parcel since it has just been deleted.
      waive(parcel);

      leave();
    }

    ///
    /// XXX
    ///
    template <typename I>
    Status		Door::Reply(const I			inputs)
    {
      Session*		session;

      enter();

      // retrieve the current session.
      if (Session::Instance(session) == StatusError)
	escape("unable to retrieve the session instance");

      // transmit a message as a response by using the event of
      // the received message i.e the current session.
      if (this->Transmit(inputs, session->event) == StatusError)
	escape("unable to transmit the reply");

      leave();
    }

  }
}

#endif
