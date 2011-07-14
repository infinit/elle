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
// updated       julien quintard   [wed jul 13 16:26:25 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Session.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variables is the session specific to the current thread/fiber.
    ///
    Session*			Session::Current = NULL;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the session.
    ///
    Status		Session::Initialize()
    {
      Callback<
	Parameters<const Phase, Fiber*> >	govern(&Session::Govern);

      enter();

      // register the govern callback to the fiber system.
      if (Fiber::Register(govern) == StatusError)
	escape("unable to register the govern callback");

      leave();
    }

    ///
    /// this method cleans the session.
    ///
    Status		Session::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method returns the instance of the session.
    ///
    Status		Session::Instance(Session*&		session)
    {
      enter();

      // return the current session.
      session = Session::Current;

      leave();
    }

    ///
    /// this method explicitely assign the session.
    ///
    Status		Session::Assign(Session*		session)
    {
      enter();

      // set the current session.
      Session::Current = session;

      leave();
    }

    ///
    /// this method is called to indicate that the session is no
    /// longer valid.
    ///
    Status		Session::Clear()
    {
      enter();

      // clear the pointer.
      Session::Current = NULL;

      leave();
    }

    ///
    /// this method initializes, saves, restores and cleans the session
    /// for the given fiber.
    ///
    Status		Session::Govern(const Phase&		phase,
					Fiber*&			fiber)
    {
      enter();

      // perform an operation depending on the phase.
      switch (phase)
	{
	case PhaseInitialize:
	  {
	    // nothing to do.

	    break;
	  }
	case PhaseSave:
	  {
	    // save the session in the fiber's environment.
	    if (fiber->environment->Store("session",
					  Session::Current) == StatusError)
	      escape("unable to store the session in the environment");

	    // set the pointer to NULL, for safety purposes.
	    Session::Current = NULL;

	    break;
	  }
	case PhaseRestore:
	  {
	    // restore the session from the fiber's environment.
	    if (fiber->environment->Load("session",
					 Session::Current) == StatusError)
	      escape("unable to load the session from the environment");

	    break;
	  }
	case PhaseClean:
	  {
	    // nothing to do.
	    //
	    // actually, at this point, the session must have been deleted
	    // by the network manager.

	    // ... but reinitializes the session pointer to make sure
	    // everything is clean!
	    Session::Current = NULL;

	    break;
	  }
	}

      leave();
    }

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

      std::cout << alignment << "[Session] " << std::hex << this << std::endl;

      // dump the socket.
      if (this->socket != NULL)
	{
	  if (this->socket->Dump(margin + 2) == StatusError)
	    escape("unable to dump the socket");
	}

      // dump the address.
      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

      // dump the event.
      if (this->event.Dump(margin + 2) == StatusError)
	escape("unable to dump the event");

      leave();
    }

  }
}
