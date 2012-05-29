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

#include <elle/concurrency/Scheduler.hh>
#include <elle/network/Session.hh>
#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {
//
// ---------- static methods --------------------------------------------------
//

    reactor::LocalStorage<Session*> Session::session(concurrency::scheduler());


    ///
    /// this method initializes the session.
    ///
    Status              Session::Initialize()
    {
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
    /// this method explicitely assign the session.
    ///
    Status              Session::Assign(Session* s)
    {
      // set the current session.
      session.Get() = s;

      return Status::Ok;
    }

    ///
    /// this method is called to indicate that the session is no
    /// longer valid.
    ///
    Status              Session::Clear()
    {
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
