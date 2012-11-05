#ifndef NUCLEUS_NEUTRON_TRAIT_HH
# define NUCLEUS_NEUTRON_TRAIT_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>
# include <elle/serialize/fwd.hh>

namespace nucleus
{
  namespace neutron
  {

    /// This class represents an attribute entry i.e a trait.
    ///
    /// A trait is characterised by a string name and while a value string
    /// is associated with it.
    class Trait:
      public elle::Printable
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return a null trait i.e a trait which represent an non-applicable
      /// value.
      static
      Trait const&
      null();

      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      enum class Type
      {
        null,
        valid
      };

      /*------.
      | Types |
      `------*/
    public:
      typedef elle::String Symbol;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Trait(); // XXX[deserialize]
      Trait(elle::String const& name,
            elle::String const& value);
      Trait(Trait const& other);
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
      /// Return the trait's value.
      elle::String const&
      value() const;
      /// Update the trait's value.
      void
      value(elle::String const& value) const;

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
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Trait);
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // rangeable
      virtual
      elle::String const&
      symbol();

      /*-----------.
      | Structures |
      `-----------*/
    public:
      struct Valid
      {
        // construction
      public:
        Valid();
        Valid(elle::String const& name,
              elle::String const& value);

      public:
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(Trait::Valid);

        // attributes
      private:
        ELLE_ATTRIBUTE_R(elle::String, name);
        ELLE_ATTRIBUTE_RW(elle::String, value);
      };

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Type, type)
      ELLE_ATTRIBUTE(Valid*, valid);
    };

  }
}

# include <nucleus/neutron/Trait.hxx>

#endif
