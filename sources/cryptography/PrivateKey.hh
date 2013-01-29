#ifndef INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH
# define INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>
# include <cryptography/interface.hh>

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/utility/fwd.hh>

# include <openssl/bn.h>
# include <openssl/evp.h>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace infinit
{
  namespace cryptography
  {
    /// Represent a private key in a given cryptosystem, RSA for instance.
    ///
    /// Note that this class could have been designed as a mere interface
    /// implemented by every cryptosystem algorithm. Unfortunately, in
    /// order to be serializable, once must keep the algorithm so as to
    /// build the right instance through a factory. This is essentially
    /// what this class does along with forwarding the calls to the
    /// appropriate implementation.
    class PrivateKey:
      public elle::concept::MakeUniquable<PrivateKey>,
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
      PrivateKey(); // XXX[deserialize]
      /// Construct a private key by providing its implementation.
      explicit
      PrivateKey(std::unique_ptr<interface::PrivateKey>&& implementation);
      PrivateKey(PrivateKey const& other);
      PrivateKey(PrivateKey&& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(PrivateKey);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Decrypt a code and returns the original clear text.
      ///
      /// Note that the code is, in practice, an archive containing both
      /// a temporarily-generated secret key and the plain text encrypted
      /// with the secret key.
      Clear
      decrypt(Code const& code) const;
      /// Decrypt a code and returns the given type, assuming the given type
      /// can be extracted from the clear, which should then be an archive.
      template <typename T>
      T
      decrypt(Code const& code) const;
      /// Return a signature of the given plain text.
      Signature
      sign(Plain const& plain) const;
      /// Return a signature of any given serializable type.
      template <typename T>
      Signature
      sign(T const& value) const;
      /// Encrypt the given plain text with the private key.
      ///
      /// Although unusual, the private key can very well be used for
      /// encrypting in which case the public key would be used for
      /// decrypting.
      Code
      encrypt(Plain const& plain) const;
      /// Encrypt the given serializable type with the private key.
      template <typename T>
      Code
      encrypt(T const& value) const;

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(PrivateKey const& other) const;
      elle::Boolean
      operator <(PrivateKey const& other) const;
      ELLE_OPERATOR_NO_ASSIGNMENT(PrivateKey);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(PrivateKey);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(std::unique_ptr<interface::PrivateKey>, implementation);
    };

    namespace privatekey
    {
      /*----------.
      | Functions |
      `----------*/

      /// Return the factory used for building private key implementations given
      /// a cryptosystem.
      elle::utility::Factory<Cryptosystem> const&
      factory();
    }
  }
}

# include <cryptography/PrivateKey.hxx>

#endif
