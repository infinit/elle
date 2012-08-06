#ifndef HOLE_IMPLEMENTATIONS_SLUG_HOST_HH
# define HOLE_IMPLEMENTATIONS_SLUG_HOST_HH

# include <elle/idiom/Close.hh>
# include <reactor/network/socket.hh>
# include <elle/idiom/Open.hh>

# include <elle/types.hh>
# include <elle/network/fwd.hh>
# include <elle/network/Locus.hh>
# include <elle/concurrency/Signal.hh>

# include <protocol/ChanneledStream.hh>
# include <protocol/Serializer.hh>

# include <hole/implementations/slug/Manifest.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {
      class Host
        : public elle::radix::Entity
      {
      public:
        // Enumerations
        enum class State
        {
          connected,
          authenticating,
          authenticated,
          dead,
        };

      /*-------------.
      | Construction |
      `-------------*/
      public:
        Host(Machine& machine, std::unique_ptr<reactor::network::Socket> socket, bool opener);
        ~Host();
      private:
        Machine& _machine;
        State _state;
        std::unique_ptr<reactor::network::Socket> _socket;
        infinit::protocol::Serializer _serializer;
        infinit::protocol::ChanneledStream _channels;
        RPC _rpcs;
        reactor::Thread _rpcs_handler;

      /*----.
      | API |
      `----*/
      public:
        std::vector<elle::network::Locus>
        authenticate(lune::Passport& passport);
        std::unique_ptr<nucleus::proton::MutableBlock>
        pull(nucleus::proton::Address const& address,
             nucleus::proton::Version const& version);
        bool
        push(nucleus::proton::Address const& address,
             nucleus::proton::Block const& block);
        bool
        wipe(nucleus::proton::Address const& address);
      private:
        std::vector<elle::network::Locus>
        _authenticate(lune::Passport const& passport);
        bool
        _push(nucleus::proton::Address const& address,
              nucleus::Derivable& derivable);
        nucleus::Derivable
        _pull(nucleus::proton::Address const&,
              nucleus::proton::Version const&);
        bool
        _wipe(nucleus::proton::Address const&);

      /*---------.
      | Dumpable |
      `---------*/
      public:
        elle::Status    Dump(const elle::Natural32 = 0) const;

      /*-------------.
      | Pretty print |
      `-------------*/
      public:
        void print(std::ostream& stream) const;
      };

      std::ostream&
      operator << (std::ostream& stream, const Host& host);
    }
  }
}

#endif
