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

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;
      };

    }
  }
}

#endif
