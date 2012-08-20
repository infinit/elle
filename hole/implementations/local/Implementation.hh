#ifndef HOLE_IMPLEMENTATIONS_LOCAL_IMPLEMENTATION_HH
# define HOLE_IMPLEMENTATIONS_LOCAL_IMPLEMENTATION_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>

# include <hole/Holeable.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {

      ///
      /// the local hole implementation.
      ///
      class Implementation:
        public Holeable
      {
      public:
        //
        // constructors & destructors
        //
        Implementation(const nucleus::proton::Network&);

        //
        // interfaces
        //

        // holeable
        void                    Join();
        elle::Status            Leave();

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

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;
      };

    }
  }
}

#endif
