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
// updated       julien quintard   [sat aug 27 00:43:46 2011]
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
      callback(NULL)
    {
    }

    ///
    /// a constructor which specifies the type of socket.
    ///
    Socket::Socket(const Socket::Type				type):
      type(type),
      callback(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Socket::~Socket()
    {
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
					  Callback<
					    Status,
					    Parameters<>
					    >&			callback)
    {
      enter();

      // delete a previous callback.
      if (this->callback != NULL)
	escape("this socket is already monitored");

      // allocate and copy a new callback.
      this->callback = new Callback< Status,
				     Parameters<> >(callback);

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

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever the socket state changes.
    ///
    Status		Socket::Signal()
    {
      enter();

      // only process the error if a monitor callback has been registered.
      if (this->callback != NULL)
	{
	  // trigger the callback.
	  if (this->callback->Call() == StatusError)
	    escape("an error occured in the callback");
	}

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Socket::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Socket] " << std::hex << this << std::endl;

      // dump the type.
      std::cout << alignment << Dumpable::Shift
		<< "[Type] " << std::dec << this->type << std::endl;

      // dump the callback.
      if (this->callback != NULL)
	{
	  if (this->callback->Dump(margin + 2) == StatusError)
	    escape("unable to dump the callback");
	}
      else
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Callback] " << none << std::endl;
	}

      leave();
    }

  }
}
