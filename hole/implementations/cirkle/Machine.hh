#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_MACHINE_HH
# define HOLE_IMPLEMENTATIONS_CIRKLE_MACHINE_HH

#include <elle/types.hh>

#include <nucleus/proton/fwd.hh>

#include <lune/fwd.hh>

#include <hole/implementations/cirkle/fwd.hh>
#include <hole/implementations/cirkle/Neighbourhood.hh>
#include <hole/implementations/cirkle/RoutingTable.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
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
        struct                                  Default
        {
          static const elle::Natural16          Port;
        };

        static const elle::Natural32            Frequency;

        //
        // enumerations
        //
        enum State
          {
            StateUnauthenticated,
            StateAuthenticated
          };

        //
        // constructors & destructors
        //
        Machine();

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
        elle::Status            Connection(elle::Gate*);

        elle::Status            Challenge();
        elle::Status            Passport(const lune::Passport&);
        elle::Status            Port(const elle::network::Port&);
        elle::Status            Authenticated();
        elle::Status            Update(const Cluster&);

        elle::Status            Gossip();

        elle::Status            Push(const nucleus::proton::Address&,
                                     const
                                       nucleus::proton::Derivable<nucleus::proton::Block>&);
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

        Neighbourhood           neighbourhood;
        RoutingTable            routingtable;

        elle::concurrency::Timer             timer;

        elle::network::Port              port;
      };

    }
  }
}

#endif
