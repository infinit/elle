//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Channel.cc
//
// created       julien quintard   [thu mar 18 21:20:27 2010]
// updated       julien quintard   [sat sep  3 11:07:22 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Channel.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this value defines the maximum capacity of a buffered packed, in bytes.
    ///
    /// \todo XXX to change to something around 10MB
    const Natural64		Channel::Capacity = 52428800;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Channel::Channel(Socket::Type				type):
      Socket(type),

      state(StateUnconnected),
      buffer(NULL),
      offset(0),
      timer(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Channel::~Channel()
    {
      Channel::Iterator	iterator;

      // delete the buffer.
      if (this->buffer != NULL)
	delete this->buffer;

      // go through the container.
      for (iterator = this->queue.begin();
	   iterator != this->queue.end();
	   iterator++)
	{
	  Parcel*	parcel = *iterator;

	  // delete the parcel.
	  delete parcel;
	}

      // clear the container.
      this->queue.clear();

      // delete the timer.
      if (this->timer != NULL)
	delete this->timer;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Channel::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Channel::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Channel]" << std::endl;

      // dump the socket.
      if (Socket::Dump(margin + 2) == StatusError)
	escape("unable to dump the socket");

      // dump the state.
      std::cout << alignment << Dumpable::Shift
		<< "[State] " << std::dec << this->state << std::endl;

      // dump the buffer.
      if (this->buffer != NULL)
	{
	  if (this->buffer->Dump(margin + 2) == StatusError)
	    escape("unable to dump the buffer");
	}

      // dump the offset.
      std::cout << alignment << Dumpable::Shift
		<< "[Offset] " << std::dec << this->offset << std::endl;

      // dump the queue.
      std::cout << alignment << Dumpable::Shift
		<< "[Queue]" << std::endl;

      // go through the queue.
      for (scoutor = this->queue.begin();
	   scoutor != this->queue.end();
	   scoutor++)
	{
	  Parcel*	parcel = *scoutor;

	  // dump the parcel.
	  if (parcel->Dump(margin + 4) == StatusError)
	    escape("unable to dump the parcel");
	}

      // dump the timer, if present.
      if (this->timer != NULL)
	{
	  if (this->timer->Dump(margin + 2) == StatusError)
	    escape("unable to dump the timer");
	}

      leave();
    }

  }
}
