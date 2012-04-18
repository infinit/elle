#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH

# include <elle/types.hh>

# include <elle/network/fwd.hh>

# include <nucleus/Nucleus.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- classes ---------------------------------------------------------
//

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
        elle::Status            Connected();
        elle::Status            Disconnected();
        elle::Status            Error(const elle::String&);

        elle::Status            Authenticated();
        elle::Status            Exception(const elle::Report&);

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
