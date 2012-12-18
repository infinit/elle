#ifndef ETOILE_GEAR_FILE_HH
# define ETOILE_GEAR_FILE_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Limits.hh>

# include <etoile/gear/Object.hh>
# include <etoile/gear/Nature.hh>

# include <etoile/nest/fwd.hh>

# include <etoile/automaton/File.hh>

# include <etoile/wall/File.hh>

namespace etoile
{
  namespace gear
  {

    ///
    /// this class represents a file-specific context.
    ///
    /// this context derives the Object context and therefore benefits from
    /// all the object-related attributes plus the contents i.e the catalog
    /// in the case of a file.
    ///
    class File:
      public Object
    {
    public:
      //
      // constants
      //
      static const Nature                       N = NatureFile;

      //
      // types
      //
      typedef wall::File W;
      typedef automaton::File A;
      typedef nucleus::neutron::Data C;

      //
      // constructors & destructors
      //
      File();
      ~File();

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      nucleus::proton::Porcupine<nucleus::neutron::Data>* porcupine;
      etoile::nest::Nest* nest;
      nucleus::proton::Limits limits;
    };

  }
}

#endif
