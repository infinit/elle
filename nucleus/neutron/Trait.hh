#ifndef NUCLEUS_NEUTRON_TRAIT_HH
# define NUCLEUS_NEUTRON_TRAIT_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents an attribute entry i.e a trait.
    ///
    /// a trait is characterised by a string name and while a value string
    /// is associated with it.
    ///
    class Trait:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Trait                Null;

      //
      // types
      //
      typedef elle::String              S;

      //
      // constructors & destructors
      //
      Trait();
      Trait(const elle::String&,
            const elle::String&);

      //
      // interfaces
      //

      // object
#include <elle/idiom/Open.hh>
      declare(Trait);
#include <elle/idiom/Close.hh>
      elle::Boolean     operator==(const Trait&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // rangeable
      elle::String&     Symbol();

      //
      // attributes
      //
      elle::String      name;
      elle::String      value;
    };

  }
}

#include <nucleus/neutron/Trait.hxx>

#endif
