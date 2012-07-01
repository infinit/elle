#ifndef  ELLE_NETWORK_SESSION_HH
# define ELLE_NETWORK_SESSION_HH

# include <elle/types.hh>

# include <elle/radix/Entity.hh>

# include <elle/concurrency/Event.hh>

# include <elle/network/Locus.hh>

# include <elle/idiom/Close.hh>
# include <elle/reactor/src/reactor/storage.hh>
# include <elle/idiom/Open.hh>

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

#endif
