#ifndef ETOILE_GEAR_LINK_HH
# define ETOILE_GEAR_LINK_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/neutron/Reference.hh>

# include <etoile/gear/Object.hh>
# include <etoile/gear/Nature.hh>

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
      typedef wall::Link                        W;
      typedef automaton::Link                   A;
      typedef elle::concurrency::Callback<
                elle::Status,
                elle::radix::Parameters<
                  Link&
                  >
                >                               S;

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
      nucleus::proton::Contents<C>* contents;
    };

  }
}

# include <etoile/gear/Link.hxx>

#endif
