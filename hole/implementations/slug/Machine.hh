#ifndef HOLE_IMPLEMENTATIONS_SLUG_MACHINE_HH
# define HOLE_IMPLEMENTATIONS_SLUG_MACHINE_HH

# include <elle/types.hh>
# include <reactor/network/tcp-server.hh>
# include <nucleus/Nucleus.hh>
# include <nucleus/Derivable.hh>
# include <lune/fwd.hh>

# include <hole/implementations/slug/Guestlist.hh>
# include <hole/implementations/slug/Neighbourhood.hh>
# include <hole/implementations/slug/Cluster.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX represents the current host
      ///
      class Machine:
        public elle::radix::Entity
      {
      public:
        //
        // constants
        //
        struct Default
        {
          static const elle::Natural16  Port;
        };

        static const reactor::Duration  Timeout;

        //
        // enumerations
        //
        enum State
        {
          StateDetached,
          StateAttached
        };

        //
        // constructors & destructors
        //
        Machine();
        ~Machine();

        //
        // methods
        //
        elle::Status            Launch();

        elle::Status            Put(const nucleus::Address&,
                                    const nucleus::ImmutableBlock&);
        elle::Status            Put(const nucleus::Address&,
                                    const nucleus::MutableBlock&);
        elle::Status            Get(const nucleus::Address&,
                                    nucleus::ImmutableBlock&);
        elle::Status            Get(const nucleus::Address&,
                                    const nucleus::Version&,
                                    nucleus::MutableBlock&);
        elle::Status            Kill(const nucleus::Address&);

        //
        // callbacks
        //
        elle::Status            Alone();

        elle::Status            Authenticate(const lune::Passport&,
                                             const elle::network::Port&);
        elle::Status            Authenticated(const Cluster&);

        elle::Status            Sweep(Host*);
        elle::Status            Synchronised();

        elle::Status            Push(const nucleus::Address&,
                                     const nucleus::Derivable&);
        elle::Status            Pull(const nucleus::Address&,
                                     const nucleus::Version&);
        elle::Status            Wipe(const nucleus::Address&);

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        State                   state;
        elle::network::Port     port;

        Guestlist               guestlist;
        Neighbourhood           neighbourhood;

      private:
        void                            _accept();
        reactor::network::TCPServer*    _server;
      };

    }
  }
}

#endif
