//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       network
//
// file          /home/mycure/infinit/elle/network/Context.cc
//
// created       julien quintard   [fri mar  5 10:52:02 2010]
// updated       julien quintard   [sun mar  7 21:12:08 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Context.hh>

namespace elle
{
  namespace network
  {

//
// ---------- globals ---------------------------------------------------------
//

    ///
    /// this variables is the context specific to the current thread.
    ///
    /// since events occur one after another, there can be conflicts.
    ///
    /// XXX \todo store this context in the thread local storage.
    ///
    Context*			context = NULL;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this constructor sets the arguments.
    ///
    Context::Context(Socket*					socket,
		     const Address&				address):
      socket(socket),
      address(address)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets the new context.
    ///
    Status		Context::Assign(Context*		c)
    {
      enter();

      // release the previous context.
      if (context != NULL)
	delete context;

      /// XXX \todo ici au lieu de faire ca, on pourrait set dans un
      /// thread local storage, ou mieux dans currentThread() qu'on
      /// static_const en Thread a nous et on set le context network.

      // assign the new context as being the current one.
      context = c;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the context.
    ///
    Status		Context::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Context]" << std::endl;

      // dump the socket.
      if (this->socket->Dump(margin + 2) == StatusError)
	escape("unable to dump the socket");

      // dump the address.
      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

      leave();
    }

  }
}
