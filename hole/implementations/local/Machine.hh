#ifndef HOLE_IMPLEMENTATIONS_LOCAL_MACHINE_HH
# define HOLE_IMPLEMENTATIONS_LOCAL_MACHINE_HH

# include <elle/types.hh>
# include <elle/radix/Entity.hh>

# include <nucleus/proton/fwd.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {

      ///
      /// this class represents the current host.
      ///
      /// note that every host can emulate multiple nodes though in the
      /// case of the local implementation, a single host is present i.e
      /// the machine.
      ///
      class Machine:
        public elle::radix::Entity
      {
      public:
        //
        // methods
        //
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

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;
      };

    }
  }
}

#endif
