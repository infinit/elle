#ifndef NUCLEUS_NEUTRON_TRAIT_HH
# define NUCLEUS_NEUTRON_TRAIT_HH

# include <elle/types.hh>
# include <elle/operator.hh>

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
    class Trait
    {
    public:
      //
      // constants
      //
      static const Trait                Null;

      //
      // types
      //
      typedef elle::String Symbol;

      //
      // constructors & destructors
      //
      Trait();
      Trait(const elle::String&,
            const elle::String&);

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Trait const& other) const;
      ELLE_OPERATOR_NEQ(Trait);
      ELLE_OPERATOR_ASSIGNMENT(Trait);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // rangeable
      elle::String&
      symbol();

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
