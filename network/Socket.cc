//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Socket.cc
//
// created       julien quintard   [wed feb  3 12:55:47 2010]
// updated       julien quintard   [sun may  2 20:39:18 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Socket.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor
    ///
    Socket::Socket():
      type(Socket::TypeUnknown),
      mode(Socket::ModeUnknown),
      callback(NULL)
    {
    }

    ///
    /// a constructor which specifies the type of socket.
    ///
    Socket::Socket(const Socket::Type				type,
		   const Socket::Mode				mode):
      type(type),
      mode(mode),
      callback(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Socket::~Socket()
    {
      Socket::Iterator	iterator;

      // go through the queue and delete every remaining parcel.
      for (iterator = this->queue.begin();
	   iterator != this->queue.end();
	   iterator++)
	delete *iterator;

      // release the callback.
      if (this->callback != NULL)
	delete this->callback;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method registers the callback that will be triggered
    /// should an error occur on the socket.
    ///
    Status		Socket::Monitor(const
					  Callback<const String>& callback)
    {
      enter();

      // delete a previous callback.
      if (this->callback != NULL)
	delete this->callback;

      // allocate and copy a new callback.
      this->callback = new Callback<const String>(callback);

      leave();
    }

    ///
    /// this method withdraws the control management by no longer
    /// forwarding the notifications.
    ///
    Status		Socket::Withdraw()
    {
      enter();

      // delete the callback.
      if (this->callback != NULL)
	delete this->callback;

      // set the callback to null.
      this->callback = NULL;

      leave();
    }

  }
}
