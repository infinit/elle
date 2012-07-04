#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH

# include <elle/types.hh>
# include <elle/radix/Entity.hh>

# include <elle/network/fwd.hh>
# include <elle/network/Locus.hh>

# include <nucleus/proton/fwd.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

      ///
      /// this class represents a client machine and is therefore used
      /// whenever the current host is acting as a client of another host
      /// i.e the server.
      ///
      class Client:
        public elle::radix::Entity
      {
      public:
        //
        // enumerations
        //
        enum State
          {
            StateUnknown,
            StateConnected,
            StateAuthenticated
          };

        //
        // constructors & destructors
        //
        Client(const elle::network::Locus&);
        ~Client();

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
        elle::Status            Authenticated();
        elle::Status            Exception(const elle::standalone::Report&);

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        State                   state;

        elle::network::Locus             locus;
        elle::network::TCPSocket*        socket;
      };

    }
  }
}

#endif
