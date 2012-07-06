#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOUR_HH
# define HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOUR_HH

# include <elle/types.hh>

# include <hole/Label.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Neighbour:
        public elle::radix::Entity
      {
      public:
        //
        // constants
        //
        static const elle::Natural32            Timeout;

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
        Neighbour();
        ~Neighbour();

        //
        // methods
        //
        elle::Status    Create(const elle::network::Locus&);
        elle::Status    Create(elle::Gate*);

        elle::Status    Connect();

        //
        // callbacks
        //
        elle::Status    Discard();

        elle::Status    Monitor();

        //
        // interfaces
        //

        // dumpable
        elle::Status    Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        State           state;

        elle::network::Locus     locus;
        Label           label;

        elle::network::Port      port;

        elle::Gate*     gate;

        elle::concurrency::Timer*    timer;
      };

    }
  }
}

#endif
