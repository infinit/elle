#ifndef ETOILE_AUTOMATON_FILE_HH
# define ETOILE_AUTOMATON_FILE_HH

# include <elle/types.hh>
# include <nucleus/neutron/fwd.hh>
# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this class provides functionalities for managing directories.
    ///
    class File
    {
    public:
      //
      // static methods
      //
      static elle::Status       Create(gear::File&);
      static elle::Status       Load(gear::File&);

      static elle::Status       Write(gear::File&,
                                      const nucleus::neutron::Offset&,
                                      const elle::standalone::Region&);
      static elle::Status       Read(gear::File&,
                                     const nucleus::neutron::Offset&,
                                     const nucleus::neutron::Size&,
                                     elle::standalone::Region&);
      static elle::Status       Adjust(gear::File&,
                                       const nucleus::neutron::Size&);

      static elle::Status       Discard(gear::File&);
      static elle::Status       Destroy(gear::File&);
      static elle::Status       Store(gear::File&);
    };

  }
}

#endif
