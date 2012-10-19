#ifndef HOLE_IMPLEMENTATIONS_SLUG_IMPLEMENTATION_HH
# define HOLE_IMPLEMENTATIONS_SLUG_IMPLEMENTATION_HH

# include <elle/network/Locus.hh>
# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>

# include <hole/Hole.hh>
# include <hole/implementations/slug/Machine.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

      /// Slug hole implementation.
      class Implementation:
        public Hole
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        Implementation(hole::storage::Storage& storage,
                       elle::Passport const& passport,
                       elle::Authority const& authority,
                       std::vector<elle::network::Locus> const& members,
                       int port,
                       reactor::Duration connection_timeout);
      private:
        ELLE_ATTRIBUTE_R(std::vector<elle::network::Locus>, members);
        ELLE_ATTRIBUTE_R(int, port);
        ELLE_ATTRIBUTE_R(reactor::Duration, connection_timeout);
        ELLE_ATTRIBUTE  (std::unique_ptr<Machine>, machine);

      /*------------.
      | Join, leave |
      `------------*/
      protected:
        virtual
        void
        _join();
        virtual
        void
        _leave();

      /*---------------.
      | Implementation |
      `---------------*/
      protected:
        virtual
        void
        _push(const nucleus::proton::Address& address,
              const nucleus::proton::ImmutableBlock& block);
        virtual
        void
        _push(const nucleus::proton::Address& address,
              const nucleus::proton::MutableBlock& block);
        virtual
        std::unique_ptr<nucleus::proton::Block>
        _pull(const nucleus::proton::Address&);
        virtual
        std::unique_ptr<nucleus::proton::Block>
        _pull(const nucleus::proton::Address&,
              const nucleus::proton::Revision&);
        virtual
        void
        _wipe(const nucleus::proton::Address& address);

      /*---------.
      | Dumpable |
      `---------*/
      public:
        elle::Status
        Dump(const elle::Natural32) const;
      };

    }
  }
}

#endif
