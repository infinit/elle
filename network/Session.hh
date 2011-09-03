//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       network
//
// file          /home/mycure/infinit/elle/network/Session.hh
//
// created       julien quintard   [fri mar  5 10:43:11 2010]
// updated       julien quintard   [sat sep  3 08:03:56 2011]
//

#ifndef ELLE_NETWORK_SESSION_HH
#define ELLE_NETWORK_SESSION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/concurrency/Fiber.hh>
#include <elle/concurrency/Phase.hh>
#include <elle/concurrency/Event.hh>

#include <elle/network/Point.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace concurrency;

  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// these classes need to be forward declared.
    ///
    class Socket;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains metadata related to an event such as the
    /// socket and sender's point for instance.
    ///
    class Session:
      public Entity
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Instance(Session*&);

      static Status	Assign(Session*);
      static Status	Clear();

      static Status	Govern(const Phase,
			       Fiber*);

      //
      // static attributes
      //
      static Session*	Current;

      //
      // constructors & destructors
      //
      Session();

      //
      // methods
      //
      Status		Create(Socket*,
			       const Point&,
			       const Event&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Socket*		socket;
      Point		point;
      Event		event;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Socket.hh>

#endif
