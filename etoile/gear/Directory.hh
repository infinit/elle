#ifndef ETOILE_GEAR_DIRECTORY_HH
# define ETOILE_GEAR_DIRECTORY_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/neutron/Catalog.hh>

# include <etoile/gear/Object.hh>
# include <etoile/gear/Nature.hh>

# include <etoile/automaton/Directory.hh>

# include <etoile/wall/Directory.hh>

namespace etoile
{
  namespace gear
  {

    ///
    /// this class represents a directory-specific context.
    ///
    /// this context derives the Object context and therefore benefits from
    /// all the object-related attributes plus the contents i.e the catalog
    /// in the case of a directory.
    ///
    class Directory:
      public Object
    {
    public:
      //
      // constants
      //
      static const Nature                       N = NatureDirectory;

      //
      // types
      //
      typedef wall::Directory W;
      typedef automaton::Directory A;
      typedef nucleus::neutron::Catalog C;

      //
      // constructors & destructors
      //
      Directory();
      ~Directory();

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      nucleus::proton::Contents<C>*     contents;
    };

  }
}

# include <etoile/gear/Directory.hxx>

#endif
