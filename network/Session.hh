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
// updated       julien quintard   [fri apr  9 00:36:57 2010]
//

#ifndef ELLE_NETWORK_SESSION_HH
#define ELLE_NETWORK_SESSION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/miscellaneous/Miscellaneous.hh>
#include <elle/concurrency/Concurrency.hh>

#include <elle/network/Socket.hh>
#include <elle/network/Address.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace miscellaneous;
  using namespace concurrency;

  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Socket;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains metadata related to an event such as the
    /// socket and sender's address for instance.
    ///
    class Session:
      public Dumpable
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

      static Status	Govern(const Phase&,
			       Fiber*&);

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
			       const Address&,
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
      Address		address;
      Event		event;
    };

  }
}

#endif
