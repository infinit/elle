//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       network
//
// author        julien quintard   [fri mar  5 10:43:11 2010]
//

#ifndef ELLE_NETWORK_SESSION_HH
#define ELLE_NETWORK_SESSION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <elle/radix/Entity.hh>

#include <elle/concurrency/Phase.hh>
#include <elle/concurrency/Event.hh>

#include <elle/network/Locus.hh>

namespace elle
{

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
    /// this class contains metadata related to a network event such as the
    /// socket and sender's locus for instance.
    ///
    class Session:
      public Entity
    {
    public:
      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();

      static Status     Assign(Session*);
      static Status     Clear();

      //
      // static attributes
      //
      static reactor::LocalStorage<Session*> session;

      //
      // constructors & destructors
      //
      Session();

      //
      // methods
      //
      Status            Create(Socket*,
                               const Locus&,
                               const Event&);

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Socket*           socket;
      Locus             locus;
      Event             event;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Socket.hh>

#endif
