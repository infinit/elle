#ifndef INFINIT_CRYPTOGRAPHY_PUBLICKEY_HH
# define INFINIT_CRYPTOGRAPHY_PUBLICKEY_HH

# include <cryptography/Plain.hh>
# include <cryptography/Code.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>
# include <cryptography/interface.hh>
# include <cryptography/rsa/fwd.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/serialize/construct.hh>
# include <elle/concept/Uniquable.hh>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace infinit
{
  namespace cryptography
  {
    /// Represent a public key in an asymmetric cryptosystem.
    ///
    /// Note that this class could have been designed as a mere interface
    /// implemented by every cryptosystem algorithm. Unfortunately, in
    /// order to be serializable, once must keep the algorithm so as to
    /// build the right instance through a factory. This is essentially
    /// what this class does along with forwarding the calls to the
    /// appropriate implementation.
    class PublicKey:
      public elle::concept::MakeUniquable<PublicKey>,
      public elle::Printable
    {
      /*--------.
      | Friends |
      `--------*/
    public:
      friend class KeyPair;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      PublicKey(); // XXX[to deserialize]
      /// Construct a public key by providing its implementation.
      explicit
      PublicKey(std::unique_ptr<interface::PublicKey>&& implementation);
      PublicKey(PublicKey const& other);
      PublicKey(PublicKey&& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(PublicKey);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Encrypt the given plain text.
      Code
      encrypt(Plain const& plain) const;
      /// Encrypt any serializable type.
      template <typename T>
      Code
      encrypt(T const& value) const;
      /// Return true if the given signature matches with the plain text.
      elle::Boolean
      verify(Signature const& signature,
             Plain const& plain) const;
      /// Return true if the given signature matches with the serializable
      /// value.
      template <typename T>
      elle::Boolean
      verify(Signature const& signature,
             T const& value) const;
      /// Decrypt the given code.
      ///
      /// Although unusual, the public key can very well be used for
      /// decrypting in which case the private key would be used for
      /// encrypting.
      Clear
      decrypt(Code const& code) const;
      /// Decrypt any serializable type.
      template <typename T>
      T
      decrypt(Code const& code) const;

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(PublicKey const& other) const;
      elle::Boolean
      operator <(PublicKey const& other) const;
      ELLE_OPERATOR_NO_ASSIGNMENT(PublicKey);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(PublicKey);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(std::unique_ptr<interface::PublicKey>, implementation);
    };
  }
}

# include <cryptography/PublicKey.hxx>

#endif
