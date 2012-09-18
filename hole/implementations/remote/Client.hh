#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH

# include <elle/types.hh>
# include <elle/radix/Entity.hh>
# include <elle/network/fwd.hh>

# include <elle/idiom/Close.hh>
# include <reactor/network/tcp-socket.hh>
# include <elle/idiom/Open.hh>

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
      /// A client machine and is therefore used whenever the current
      /// host is acting as a client of another host i.e the server.
      class Client
        : public elle::radix::Entity
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        Client(std::string const& host, int port);
        ~Client();

      /*----.
      | API |
      `----*/
      public:
        /// Store an immutable block.
        void
        put(const nucleus::proton::Address&,
            const nucleus::proton::ImmutableBlock&);
        /// Store a mutable block.
        void
        put(const nucleus::proton::Address&,
            const nucleus::proton::MutableBlock&);
        /// Retrieve an immutable block.
        std::unique_ptr<nucleus::proton::Block>
        get(const nucleus::proton::Address&);
        /// Retrieve a mutable block.
        std::unique_ptr<nucleus::proton::Block>
        get(const nucleus::proton::Address&, const nucleus::proton::Revision&);
        /// Remove a block.
        void
        kill(const nucleus::proton::Address&);

      /*---------.
      | Dumpable |
      `---------*/
      public:
        elle::Status
        Dump(const elle::Natural32 = 0) const;

      /*------.
      | State |
      `------*/
      private:
        enum class State
        {
          connected,
          authenticated
        };
        State _state;

      /*--------.
      | Details |
      `--------*/
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
