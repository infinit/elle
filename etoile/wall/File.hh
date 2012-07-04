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
    public:
      //
      // static methods
      //
      static elle::Status       Create(gear::Identifier&);
      static elle::Status       Load(const path::Chemin&,
                                     gear::Identifier&);

      static elle::Status       Lock(const gear::Identifier&);
      static elle::Status       Release(const gear::Identifier&);
      static elle::Status       Write(const gear::Identifier&,
                                      const nucleus::neutron::Offset&,
                                      const elle::standalone::Region&);
      static elle::Status       Read(const gear::Identifier&,
                                     const nucleus::neutron::Offset&,
                                     const nucleus::neutron::Size&,
                                     elle::standalone::Region&);
      static elle::Status       Adjust(const gear::Identifier&,
                                       const nucleus::neutron::Size&);

      static elle::Status       Discard(const gear::Identifier&);
      static elle::Status       Store(const gear::Identifier&);
      static elle::Status       Destroy(const gear::Identifier&);
      static elle::Status       Purge(const gear::Identifier&);
    };

  }
}

#endif
