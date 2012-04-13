//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       network
//
// author        julien quintard   [fri mar  5 10:52:02 2010]
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
    Session*                    Session::Current = NULL;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the session.
    ///
    Status              Session::Initialize()
    {
      // register the govern callback to the fiber system.
      if (Fiber::Register(Callback<>::Infer(&Session::Govern)) == Status::Error)
        escape("unable to register the govern callback");

      return Status::Ok;
    }

    ///
    /// this method cleans the session.
    ///
    Status              Session::Clean()
    {
      // nothing to do.

      return Status::Ok;
    }

    ///
    /// this method returns the instance of the session.
    ///
    Status              Session::Instance(Session*&             session)
    {
      // return the current session.
      session = Session::Current;

      return Status::Ok;
    }

    ///
    /// this method explicitely assign the session.
    ///
    Status              Session::Assign(Session*                session)
    {
      // set the current session.
      Session::Current = session;

      return Status::Ok;
    }

    ///
    /// this method is called to indicate that the session is no
    /// longer valid.
    ///
    Status              Session::Clear()
    {
      // clear the locuser.
      Session::Current = NULL;

      return Status::Ok;
    }

    ///
    /// this method initializes, saves, restores and cleans the session
    /// for the given fiber.
    ///
    Status              Session::Govern(const Phase             phase,
                                        Fiber*                  fiber)
    {
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
                                          Session::Current) == Status::Error)
              escape("unable to store the session in the environment");

            // set the locuser to NULL, for safety purposes.
            Session::Current = NULL;

            break;
          }
        case PhaseRestore:
          {
            // restore the session from the fiber's environment.
            if (fiber->environment->Load("session",
                                         Session::Current) == Status::Error)
              escape("unable to load the session from the environment");

            break;
          }
        case PhaseClean:
          {
            // nothing to do.
            //
            // actually, at this locus, the session must have been deleted
            // by the network manager.

            // ... but reinitializes the session locuser to make sure
            // everything is clean!
            Session::Current = NULL;

            break;
          }
        }

      return Status::Ok;
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
    Status              Session::Create(Socket*                 socket,
                                        const Locus&            locus,
                                        const Event&            event)
    {
      // set the attributes.
      this->socket = socket;
      this->locus = locus;
      this->event = event;

      return Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the session.
    ///
    Status              Session::Dump(const Natural32           margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Session] " << std::hex << this << std::endl;

      // dump the socket.
      if (this->socket != NULL)
        {
          std::cout << alignment << Dumpable::Shift
                    << "[Socket] " << std::hex << this->socket << std::endl;
        }

      // dump the locus.
      if (this->locus.Dump(margin + 2) == Status::Error)
        escape("unable to dump the locus");

      // dump the event.
      if (this->event.Dump(margin + 2) == Status::Error)
        escape("unable to dump the event");

      return Status::Ok;
    }

  }
}
