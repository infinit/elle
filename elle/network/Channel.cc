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
// updated       julien quintard   [thu jul 14 14:13:48 2011]
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
    const Natural64		Channel::Capacity = 5242880;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Channel::Channel(Socket::Type				type):
      Socket(type),

      buffer(NULL),
      offset(0)
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
    }

  }
}
