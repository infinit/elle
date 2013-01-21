#ifndef NUCLEUS_NEUTRON_FELLOW_HH
# define NUCLEUS_NEUTRON_FELLOW_HH

# include <elle/types.hh>
# include <elle/Printable.hh>
# include <elle/io/Dumpable.hh>

# include <nucleus/neutron/Subject.hh>
# include <nucleus/neutron/Token.hh>

namespace nucleus
{
  namespace neutron
  {
    /// Represent a group member and is defined by a subject (which can
    /// be either a user or a group i.e a subgroup), and a token which
    /// is the encrypted version of the group's private pass.
    class Fellow:
      public elle::Printable,
      public elle::io::Dumpable
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return a null fellow i.e a fellow which represent an non-applicable
      /// value.
      static
      Fellow const&
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

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Fellow(); // XXX[deserialize]
      /// Construct a fellow with a subject and its associated token.
      Fellow(Subject const& subject,
             Token const& token);
      Fellow(Fellow const& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Fellow);
      ~Fellow();
    private:
      Fellow(Type const type);

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Fellow const& other) const;
      ELLE_OPERATOR_NEQ(Fellow);
      ELLE_OPERATOR_NO_ASSIGNMENT(Fellow);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Return the fellow's subject.
      Subject const&
      subject() const;
      /// Return the fellow's token.
      Token const&
      token() const;
      /// Update the fellow's token.
      void
      token(Token const& token);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Fellow);
      ELLE_SERIALIZE_FRIEND_FOR(Ensemble);
      // rangeable
      typedef Subject Symbol;
      virtual
      Subject const&
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
        Valid(Subject const& subject,
              Token const& token);
        ~Valid();

        // methods
      public:
        Token const&
        token() const;
        void
        token(Token const& token);

      public:
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(Fellow::Valid);

        // attributes
      private:
        ELLE_ATTRIBUTE_R(Subject, subject);
        /// Note that contrary to the Record class, every
        /// fellow has a token since such a token protects
        /// the group's pass. Since every group has a pass,
        /// no matter the revision of the group, fellows have
        /// a token.
        /// However, since once must be able to re-generate
        /// a token, following the modification of the group's
        /// pass for example, the token is represented through
        /// a pointer.
        ELLE_ATTRIBUTE(Token*, token);
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
                Fellow::Type const type);
  }
}

# include <nucleus/neutron/Fellow.hxx>

#endif
