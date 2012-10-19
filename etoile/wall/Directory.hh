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
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Creates a directory though orphan since not attached to the hierarchy.
      static
      gear::Identifier
      create();
      /// Loads the directory referenced through the given chemin.
      static
      gear::Identifier
      load(path::Chemin const& chemin);

      static elle::Status       Lock(const gear::Identifier&);
      static elle::Status       Release(const gear::Identifier&);

      /// Adds an entry to the given directory.
      static
      void
      add(gear::Identifier const& parent,
          path::Slab const& name,
          gear::Identifier const& child);

      static elle::Status       Lookup(const gear::Identifier&,
                                       const path::Slab&,
                                       nucleus::neutron::Entry const*&);
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

      /// Commit the pending modifications by placing the scope in the journal.
      static
      void
      store(gear::Identifier const& identifier);

      static elle::Status       Destroy(const gear::Identifier&);
      static elle::Status       Purge(const gear::Identifier&);
    };

  }
}

#endif
