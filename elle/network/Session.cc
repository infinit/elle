//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       network
//
// file          /home/mycure/infinit/elle/network/Session.cc
//
// created       julien quintard   [fri mar  5 10:52:02 2010]
// updated       julien quintard   [thu mar 25 17:40:48 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Session.hh>

namespace elle
{
  namespace network
  {

//
// ---------- globals ---------------------------------------------------------
//

    ///
    /// this variables is the session specific to the current thread.
    ///
    /// since events occur one after another, there can be conflicts.
    ///
    /// XXX \todo store this session in the thread local storage.
    ///
    Session*			session = NULL;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Session::Session():
      socket(NULL)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets the arguments.
    ///
    Status		Session::Create(Socket*			socket,
					const Address&		address,
					const Event&		event)
    {
      enter();

      // set the attributes.
      this->socket = socket;
      this->address = address;
      this->event = event;

      leave();
    }

    ///
    /// this method sets the new session.
    ///
    Status		Session::Assign(Session*		s)
    {
      enter();

      /// XXX \todo ici au lieu de faire ca, on pourrait set dans un
      /// thread local storage, ou mieux dans currentThread() qu'on
      /// static_const en Thread a nous et on set le session network.

      // assign the new session as being the current one.
      session = s;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the session.
    ///
    Status		Session::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Session]" << std::endl;

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
