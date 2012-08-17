#ifndef ETOILE_AUTOMATON_DIRECTORY_HH
# define ETOILE_AUTOMATON_DIRECTORY_HH

# include <elle/types.hh>
# include <nucleus/fwd.hh>
# include <etoile/path/fwd.hh>
# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this class provides a set of automata for operating on directory
    /// contexts.
    ///
    class Directory
    {
    public:
      //
      // static methods
      //
      static elle::Status       Create(gear::Directory&);
      static elle::Status       Load(gear::Directory&);

      static elle::Status       Add(gear::Directory&,
                                    const path::Slice&,
                                    const nucleus::proton::Address&);
      static elle::Status       Lookup(gear::Directory&,
                                       const path::Slice&,
                                       nucleus::neutron::Entry const*&);
      static elle::Status       Consult(gear::Directory&,
                                        const nucleus::neutron::Index&,
                                        const nucleus::neutron::Size&,
                                        nucleus::neutron::Range<
                                          nucleus::neutron::Entry>&);
      static elle::Status       Rename(gear::Directory&,
                                       const path::Slice&,
                                       const path::Slice&);
      static elle::Status       Remove(gear::Directory&,
                                       const path::Slice&);

      static elle::Status       Discard(gear::Directory&);
      static elle::Status       Destroy(gear::Directory&);
      static elle::Status       Store(gear::Directory&);
    };

  }
}

#endif
