#ifndef NUCLEUS_NEUTRON_TRAIT_HH
# define NUCLEUS_NEUTRON_TRAIT_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>

# include <nucleus/proton/Footprint.hh>
# include <nucleus/neutron/fwd.hh>

namespace nucleus
{
  namespace neutron
  {
    /// Represent an extended attribute characterised by a name and string-based
    /// value.
    class Trait:
      public elle::Printable
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return a null trait i.e a trait which represents an non-applicable
      /// value.
      static
      Trait const&
      null();

      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      /// Define the type of trait either valid or not.
      enum class Type
        {
          null,
          valid
        };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Trait(); // XXX[deserialize]
      Trait(elle::String const& name,
            elle::String const& value);
      Trait(Trait const& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Trait);
      ~Trait();
    private:
      Trait(Type const type);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Return the trait's name.
      elle::String const&
      name() const;
      /// Update the trait's name.
      void
      name(elle::String const& name);
      /// Return the trait's value.
      elle::String const&
      value() const;
      /// Update the trait's value.
      void
      value(elle::String const& value);

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Trait const& other) const;
      ELLE_OPERATOR_NEQ(Trait);
      ELLE_OPERATOR_ASSIGNMENT(Trait); // XXX

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Trait);
      ELLE_SERIALIZE_FRIEND_FOR(Attributes);
      // rangeable
      typedef elle::String Symbol;
      virtual
      elle::String const&
      symbol() const;

      /*-----------.
      | Structures |
      `-----------*/
    public:
      struct Valid
      {
        // construction
      public:
        Valid(); // XXX
        Valid(elle::String const& name,
              elle::String const& value);
        Valid(Valid const& other) = default;

      public:
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(Trait::Valid);

        // attributes
      private:
        ELLE_ATTRIBUTE_RW(elle::String, name);
        ELLE_ATTRIBUTE_RW(elle::String, value);
      };

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Type, type)
      ELLE_ATTRIBUTE(Valid*, valid);
      ELLE_ATTRIBUTE_R(proton::Footprint, footprint);
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Trait::Type const type);
  }
}

# include <nucleus/neutron/Trait.hxx>

#endif
