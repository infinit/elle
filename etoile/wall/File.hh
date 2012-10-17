#ifndef ETOILE_WALL_FILE_HH
# define ETOILE_WALL_FILE_HH

# include <elle/types.hh>
# include <elle/standalone/fwd.hh>

# include <nucleus/neutron/fwd.hh>

# include <etoile/path/fwd.hh>
# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace wall
  {

    ///
    /// this class provides functionalities for managing file objects.
    ///
    class File
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// This method creates a new file object.
      ///
      /// Note however that the object is not attached to the hierarchy
      /// and is therefore considered as orphan.
      static
      gear::Identifier
      create();

      static elle::Status       Load(const path::Chemin&,
                                     gear::Identifier&);

      static elle::Status       Lock(const gear::Identifier&);
      static elle::Status       Release(const gear::Identifier&);

      /// Writes the file with the given region of data.
      static
      void
      write(gear::Identifier const& identifier,
            nucleus::neutron::Offset const& offset,
            elle::standalone::Region const& data);

      static elle::Status       Read(const gear::Identifier&,
                                     const nucleus::neutron::Offset&,
                                     const nucleus::neutron::Size&,
                                     elle::standalone::Region&);
      static elle::Status       Adjust(const gear::Identifier&,
                                       const nucleus::neutron::Size&);

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
