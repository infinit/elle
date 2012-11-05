#ifndef NUCLEUS_NEUTRON_TOKEN_HH
# define NUCLEUS_NEUTRON_TOKEN_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>
# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/Code.hh>

namespace nucleus
{
  namespace neutron
  {

    /// A token is a secret information enabling a user to access encrypted
    /// data. However, in order to allow only the proper user to
    /// use this secret information, it is encrypted with the user's public
    /// key.
    class Token:
      public elle::Printable
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return an unused token.
      static
      Token const&
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
      Token();
      template <typename T>
      Token(T const& secret,
            elle::cryptography::PublicKey const& K);
      Token(Token const& other);
      ~Token();
    private:
      Token(Type const type);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Extract the secret information from the token by decrypting
      /// the code with the given private key which only the user has.
      template <typename T>
      T
      extract(elle::cryptography::PrivateKey const& k) const;

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Token const& other) const;
      ELLE_OPERATOR_NEQ(Token);
      ELLE_OPERATOR_ASSIGNMENT(Token); // XXX

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Token);

      /*-----------.
      | Structures |
      `-----------*/
    public:
      struct Valid
      {
        // construction
      public:
        Valid();
        Valid(elle::cryptography::Code const& code);

      public:
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(Token::Valid);

        // attributes
      private:
        ELLE_ATTRIBUTE_R(elle::cryptography::Code, code);
      };

      /*-----------.
      | Attributes |
      `-----------*/
    public:
      ELLE_ATTRIBUTE_R(Type, type);
      ELLE_ATTRIBUTE(Valid*, valid);
    };

  }
}

# include <nucleus/neutron/Token.hxx>

#endif
