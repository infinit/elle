#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH

# include <elle/types.hh>
# include <elle/radix/Entity.hh>
# include <elle/network/fwd.hh>

# include <reactor/network/tcp-socket.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/Nucleus.hh>
# include <hole/implementations/remote/Manifest.hh>

# include <protocol/ChanneledStream.hh>
# include <protocol/Serializer.hh>

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

      /*-------------.
      | Construction |
      `-------------*/
      public:
        Client(std::string const& host, int port);
        ~Client();

        //
        // methods
        //
        elle::Status            Launch();

        /// Store an immutable block.
        void Put(const nucleus::proton::Address&, const nucleus::proton::ImmutableBlock&);
        /// Store a mutable block.
        void Put(const nucleus::proton::Address&, const nucleus::proton::MutableBlock&);
        /// Retrieve an immutable block.
        std::unique_ptr<nucleus::proton::Block>
        Get(const nucleus::proton::Address&);
        /// Retrieve a mutable block.
        std::unique_ptr<nucleus::proton::Block>
        Get(const nucleus::proton::Address&, const nucleus::proton::Version&);
        /// Remove a block.
        void Kill(const nucleus::proton::Address&);

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

      private:
        reactor::network::TCPSocket _stream;
        infinit::protocol::Serializer _serializer;
        infinit::protocol::ChanneledStream _channels;
        RPC _rpc;
      };

    }
  }
}

#endif
