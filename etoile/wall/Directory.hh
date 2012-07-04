#ifndef ETOILE_WALL_DIRECTORY_HH
# define ETOILE_WALL_DIRECTORY_HH

# include <elle/types.hh>

# include <nucleus/neutron/fwd.hh>

# include <etoile/path/fwd.hh>
# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace wall
  {

    ///
    /// this class provides an interface for manipulating directories.
    ///
    class Directory
    {
    public:
      //
      // static methods
      //
      static elle::Status       Create(gear::Identifier&);
      static elle::Status       Load(const path::Chemin&,
                                     gear::Identifier&);

      static elle::Status       Lock(const gear::Identifier&);
      static elle::Status       Release(const gear::Identifier&);
      static elle::Status       Add(const gear::Identifier&,
                                    const path::Slab&,
                                    const gear::Identifier&);
      static elle::Status       Lookup(const gear::Identifier&,
                                       const path::Slab&,
                                       nucleus::neutron::Entry*&);
      static elle::Status       Consult(const gear::Identifier&,
                                        const nucleus::neutron::Index&,
                                        const nucleus::neutron::Size&,
                                        nucleus::neutron::Range<
                                          nucleus::neutron::Entry>&);
      static elle::Status       Rename(const gear::Identifier&,
                                       const path::Slab&,
                                       const path::Slab&);
      static elle::Status       Remove(const gear::Identifier&,
                                       const path::Slab&);

      static elle::Status       Discard(const gear::Identifier&);
      static elle::Status       Store(const gear::Identifier&);
      static elle::Status       Destroy(const gear::Identifier&);
      static elle::Status       Purge(const gear::Identifier&);
    };

  }
}

#endif
