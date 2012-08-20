#ifndef HOLE_IMPLEMENTATIONS_SLUG_MACHINE_HH
# define HOLE_IMPLEMENTATIONS_SLUG_MACHINE_HH

# include <unordered_map>

# include <elle/network/Locus.hh>
# include <elle/radix/Entity.hh>
# include <elle/types.hh>

# include <elle/idiom/Close.hh>
#  include <reactor/network/fwd.hh>
#  include <reactor/duration.hh>
# include <elle/idiom/Open.hh>

# include <nucleus/fwd.hh>

# include <lune/fwd.hh>

# include <hole/implementations/slug/fwd.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

      ///
      /// XXX represents the current host
      ///
      class Machine
        : public elle::radix::Entity
        , public elle::Printable
      {
      public:
        // constants
        static const reactor::Duration  Timeout;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        Machine();
        ~Machine();

      /*------.
      | State |
      `------*/
      public:
        enum class State
        {
          detached,
          attached
        };
      private:
        State _state;

      /*------.
      | Hosts |
      `------*/
      public:
        std::vector<elle::network::Locus> loci();
        std::vector<Host*> hosts();
      private:
        friend class Host;
        std::unordered_map<elle::network::Locus, Host*> _hosts;
        void _connect(elle::network::Locus const& locus);
        void _connect(std::unique_ptr<reactor::network::Socket> socket,
                      elle::network::Locus const& locus, bool opener);
        void _remove(Host* host);

      /*-------.
      | Server |
      `-------*/
      public:
        elle::network::Port port() const;
      private:
        elle::network::Port _port;
        void _accept();
        std::unique_ptr<reactor::network::TCPServer> _server;

      /*----.
      | API |
      `----*/
      public:
        /// Store an immutable block.
        void Put(const nucleus::proton::Address&, const nucleus::proton::ImmutableBlock&);
        /// Store a mutable block.
        void Put(const nucleus::proton::Address&, const nucleus::proton::MutableBlock&);
        /// Retrieves an immutable block.
        std::unique_ptr<nucleus::proton::Block>
        Get(const nucleus::proton::Address&);
        /// Retrieves a mutable block.
        std::unique_ptr<nucleus::proton::Block>
        Get(const nucleus::proton::Address&, const nucleus::proton::Version&);
        /// Remove a block.
        void Kill(const nucleus::proton::Address&);
      private:
        std::unique_ptr<nucleus::proton::Block>
        _get_latest(nucleus::proton::Address const&);
        std::unique_ptr<nucleus::proton::Block>
        _get_specific(nucleus::proton::Address const&, nucleus::proton::Version const&);

      /*----------.
      | Printable |
      `----------*/
      public:
        virtual void print(std::ostream& s) const;

      /*---------.
      | Dumpable |
      `---------*/
      public:
        elle::Status            Dump(const elle::Natural32 = 0) const;
      };

      std::ostream&
      operator << (std::ostream& stream, Machine::State state);
    }
  }
}

#endif
