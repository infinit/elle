#ifndef INFINIT_CRYPTOGRAPHY_PUBLICKEY_HH
# define INFINIT_CRYPTOGRAPHY_PUBLICKEY_HH

# include <cryptography/Plain.hh>
# include <cryptography/Code.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>

# include <elle/types.hh>
# include <elle/Printable.hh>
# include <elle/radix/Object.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/Buffer.hh>

# include <elle/idiom/Close.hh>
#  include <openssl/rsa.h>
#  include <openssl/bn.h>
#  include <openssl/err.h>
#  include <openssl/evp.h>
# include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {

    ///
    /// this class represents a public key.
    ///
    class PublicKey:
      public elle::radix::Object,
      public elle::concept::MakeUniquable<PublicKey>,
      public elle::Printable
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static const oneway::Algorithm oneway_algorithm;
      };

    public:
      //
      // constructors & destructors
      //
      PublicKey();
      PublicKey(const PublicKey&);
      ~PublicKey();

      //
      // methods
      //
      elle::Status            Create(const ::EVP_PKEY*);
      elle::Status            Create(elle::Large*,
                               elle::Large*);

      // XXX harmoniser les methodes encrypt/decrypt/sign/verify
      // pour prendre des Plain sans les serializer d'abord.

    public:
      ///
      /// this method encrypts the given data with the public key.
      ///
      /// since the public key size limits the size of the data that can be
      /// encrypted and raising large data to large exponent is very slow; the
      /// algorithm below consists in (i) generating a secret key, (ii)
      /// ciphering the plain text with this key, (iii) encrypting the secret
      /// key with the public key and finally (iv) returning an archive
      /// containing the asymetrically-encrypted secret key with the
      /// symmetrically-encrypted data.
      ///
      /// note however that should the plain be small enough, (1) a direct
      /// computation is performed. otherwise, (2) the data is symmetrically
      /// encrypted.
      ///
      elle::Status
      Encrypt(elle::WeakBuffer const& buffer, Code& out) const;
      template <typename T>
      elle::Status
      Encrypt(T const& in, Code& out) const;

    public:
      ///
      /// this method decrypts a code which should actually be an archive
      /// containing both a secret key and some data.
      ///
      /// note that, although it may sound strange to 'decrypt' with a public
      /// key, it does not matter as both keys act as the other's opposite.
      ///
      /// therefore the public key can be used to encrypt in which case the
      /// private key will be used for decrypting or the other way around,
      /// which is what this function is for.
      ///
      /// this method starts by (i) extracting the key and data in their
      /// encrypted forms (ii) decrypt the symmetric key with the public key
      /// and (iii) decipher the data with the symmetric key.
      ///
      elle::Status
      Decrypt(Code const& in, elle::Buffer& out) const;
      template <typename T>
      elle::Status
      Decrypt(Code const& in, T& out) const;

    public:
      ///
      /// this method verifies that the non-signed text is equal to the plain
      /// text.
      ///
      /// note that, as for the Sign() method, this method computes the plain's
      /// digest before forwarding to the other Verify() method.
      ///
      // XXX[should take Signature, Code]
      elle::Status
      Verify(Signature const& signature,
             elle::WeakBuffer const& plain) const;
      // XXX[should take Signature, Plain]
      template <typename T>
      elle::Status
      Verify(Signature const& signature,
             T const& any) const;

      //
      // interfaces
      //
    public:
      // object
      declare(PublicKey);
      // XXX ELLE_OPERATOR_ASSIGNMENT(PrivateKey); // XXX
      elle::Boolean
      operator ==(PublicKey const& other) const;
      elle::Boolean
      operator <(PublicKey const& other) const;

      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;

      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
    private:
      ::EVP_PKEY*       _key;

      struct
      {
        ::EVP_PKEY_CTX* encrypt;
        ::EVP_PKEY_CTX* verify;
        ::EVP_PKEY_CTX* decrypt;
      }                 _contexts;

    public:
      //
      // properties
      //
      ::EVP_PKEY const* key() const { return this->_key; }
    };

  }
}

#include <cryptography/PublicKey.hxx>

#endif
