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
// updated       julien quintard   [tue sep  6 12:09:41 2011]
//

#ifndef ELLE_NETWORK_DOOR_HXX
#define ELLE_NETWORK_DOOR_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/network/Packet.hh>
#include <elle/network/Header.hh>
#include <elle/network/Data.hh>
#include <elle/network/Parcel.hh>

#include <elle/Manifest.hh>

namespace elle
{
  namespace network
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sends a packet.
    ///
    template <typename I>
    Status		Door::Send(const I			inputs,
				   const Event&			event)
    {
      Packet		packet;
      Data		data;
      Header		header;

      enter();

      // create an data for the inputs.
      if (data.Create() == StatusError)
	escape("unable to create the data");

      // serialize the inputs.
      if (inputs.Serialize(data) == StatusError)
	escape("unable to serialize the inputs");

      // create the header now that we know that final archive's size.
      if (header.Create(event,
			inputs.tag,
			data.size) == StatusError)
	escape("unable to create the header");

      // prepare the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the the header and data.
      if (packet.Serialize(header, data) == StatusError)
	escape("unable to serialize the header and data");

      // write the socket.
      if (this->Write(packet) == StatusError)
	escape("unable to write the packet");

      leave();
    }

    ///
    /// this method receives a packet through blocking.
    ///
    template <typename O>
    Status		Door::Receive(const Event&		event,
				      O				outputs)
    {
      Parcel*		parcel;

      enter(instance(parcel));

      // block the current fiber until the given event is received.
      if (Fiber::Wait(event, parcel) == StatusError)
	escape("an error occured while waiting for a specific event");

      // check the tag.
      if (parcel->header->tag != outputs.tag)
	{
	  // test if the message received is an error, if so, append
	  // the report to the local one.
	  if (parcel->header->tag == TagError)
	    {
	      Report	report;

	      // extract the error message.
	      if (report.Extract(*parcel->data) == StatusError)
		escape("unable to extract the error message");

	      // report the remote error.
	      transpose(report);
	    }

	  escape("received a packet with an unexpected tag '%u'",
		 parcel->header->tag);
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
    /// this method sends a message and waits for a response.
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

      // send the inputs.
      if (this->Send(inputs, event) == StatusError)
	escape("unable to send the inputs");

      // wait for the reply.
      if (this->Receive(event, outputs) == StatusError)
	escape("unable to receive the outputs");

      leave();
    }

    ///
    /// this method replies to the message which has just been received i.e
    /// whose tag is specified in the current session.
    ///
    template <typename I>
    Status		Door::Reply(const I			inputs,
				    Session*			session)
    {
      enter();

      // retrieve the current session, if necessary.
      if (session == NULL)
	{
	  if (Session::Instance(session) == StatusError)
	    escape("unable to retrieve the session instance");
	}

      // send a message as a response by using the event of
      // the received message i.e the current session.
      if (this->Send(inputs, session->event) == StatusError)
	escape("unable to send the reply");

      leave();
    }

  }
}

#endif
