#ifndef HOLE_IMPLEMENTATIONS_LOCAL_IMPLEMENTATION_HH
# define HOLE_IMPLEMENTATIONS_LOCAL_IMPLEMENTATION_HH

# include <elle/fwd.hh>
# include <elle/types.hh>

# include <hole/Hole.hh>

# include <nucleus/proton/fwd.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {
      /// Local hole implementation.
      class Implementation:
        public Hole
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        Implementation(storage::Storage& storage,
                       elle::Passport const& passport,
                       elle::Authority const& authority);

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
        void
        _push(nucleus::proton::Address const&,
              nucleus::proton::ImmutableBlock const&);
        void
        _push(nucleus::proton::Address const&,
              nucleus::proton::MutableBlock const&);
        std::unique_ptr<nucleus::proton::Block>
        _pull(nucleus::proton::Address const&);
        std::unique_ptr<nucleus::proton::Block>
        _pull(nucleus::proton::Address const&,
              nucleus::proton::Revision const&);
        void
        _wipe(nucleus::proton::Address const&);

      /*---------.
      | Dumpable |
      `---------*/
      public:
        elle::Status
        Dump(const elle::Natural32 = 0) const;
      };

    }
  }
}

#endif
