#ifndef HOLE_IMPLEMENTATIONS_SLUG_MACHINE_HH
# define HOLE_IMPLEMENTATIONS_SLUG_MACHINE_HH

# include <elle/types.hh>
# include <elle/radix/Entity.hh>

# include <elle/idiom/Close.hh>
#  include <reactor/network/fwd.hh>
#  include <reactor/duration.hh>
# include <elle/idiom/Open.hh>

# include <nucleus/fwd.hh>

# include <lune/fwd.hh>

# include <hole/implementations/slug/fwd.hh>
# include <hole/implementations/slug/Guestlist.hh>
# include <hole/implementations/slug/Neighbourhood.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

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

        elle::Status            Put(const nucleus::proton::Address&,
                                    const nucleus::proton::ImmutableBlock&);
        elle::Status            Put(const nucleus::proton::Address&,
                                    const nucleus::proton::MutableBlock&);
        elle::Status            Get(const nucleus::proton::Address&,
                                    nucleus::proton::ImmutableBlock&);
        elle::Status            Get(const nucleus::proton::Address&,
                                    const nucleus::proton::Version&,
                                    nucleus::proton::MutableBlock&);
        elle::Status            Kill(const nucleus::proton::Address&);

        //
        // callbacks
        //
        elle::Status            Alone();

        elle::Status            Authenticate(const lune::Passport&,
                                             const elle::network::Port&);
        elle::Status            Authenticated(const Cluster&);

        elle::Status            Sweep(Host*);
        elle::Status            Synchronised();

        elle::Status            Push(const nucleus::proton::Address&,
                                     const nucleus::Derivable&);
        elle::Status            Pull(const nucleus::proton::Address&,
                                     const nucleus::proton::Version&);
        elle::Status            Wipe(const nucleus::proton::Address&);

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
