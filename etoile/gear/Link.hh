#ifndef ETOILE_GEAR_LINK_HH
# define ETOILE_GEAR_LINK_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>

# include <etoile/gear/Object.hh>
# include <etoile/gear/Nature.hh>

# include <etoile/nest/fwd.hh>

# include <etoile/automaton/Link.hh>

# include <etoile/wall/Link.hh>

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
    class Link:
      public Object
    {
    public:
      //
      // constants
      //
      static const Nature                       N = NatureLink;

      //
      // types
      //
      typedef wall::Link W;
      typedef automaton::Link A;
      typedef nucleus::neutron::Reference C;

      //
      // constructors & destructors
      //
      Link();
      ~Link();

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      nucleus::proton::Porcupine<nucleus::neutron::Reference>* porcupine;
      etoile::nest::Nest* nest;
    };

  }
}

#endif
