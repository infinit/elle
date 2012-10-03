#ifndef HOLE_IMPLEMENTATIONS_SLUG_IMPLEMENTATION_HH
# define HOLE_IMPLEMENTATIONS_SLUG_IMPLEMENTATION_HH

# include <elle/network/Locus.hh>
# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>

# include <hole/Hole.hh>

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
                       std::vector<elle::network::Locus> const& members,
                       int port);
      private:
        ELLE_ATTRIBUTE_R(std::vector<elle::network::Locus>, members);
        ELLE_ATTRIBUTE_R(int, port);

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
