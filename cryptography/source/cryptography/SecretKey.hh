#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HH
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HH

# include <elle/fwd.hh>
# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/serialize/construct.hh>

# include <cryptography/fwd.hh>

# include <openssl/evp.h>
# include <openssl/err.h>
# include <openssl/rand.h>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace infinit
{
  namespace cryptography
  {
    /// Represents a secret key for symmetric cryptosystem operations.
    class SecretKey:
      public elle::Printable
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        /// Define the magic which is embedded in every encrypted
        /// text so for the decryption process to know that the text
        /// has been salted.
        static elle::Character const magic[];
      };

      /// XXX[use enums and/or rename]
      struct Algorithms
      {
        static const ::EVP_CIPHER*      Cipher;
        static const ::EVP_MD*          Digest;
      };

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return a freshly generated secret key.
      static
      SecretKey
      generate(elle::Natural32 const length);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      SecretKey(); // XXX[to deserialize]
      /// Construct a secret key based on a string-based password.
      explicit
      SecretKey(elle::String const& password);
      /// Copy constructor.
      SecretKey(SecretKey const& other);
      /// Derialization constructor.
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(SecretKey);
    private:
      /// Construct a secret key based on a given buffer.
      SecretKey(elle::Buffer&& buffer);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Encrypt the given plain text and return a cipher text.
      Cipher
      encrypt(Plain const& plain) const;
      /// Encrypt any serializable object and return a cipher text
      /// of its archive.
      template <typename T>
      Cipher
      encrypt(T const& value) const;
      /// Decrypt the given cipher and return a clear text i.e the
      /// equivalent of the plain text provided as input to the
      /// encryption process.
      Clear
      decrypt(Cipher const& cipher) const;
      /// Decrypt a cipher and return a deserializable object.
      template <typename T>
      T
      decrypt(Cipher const& cipher) const;

      /// XXX[temporary until the nucleus node factory is set to handle this
      //      case]
      template <typename T>
      void
      decrypt(Cipher const& cipher,
              T& object) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
        ELLE_TRACE_METHOD(cipher);

        // Decrypt the cipher leading to a clear containing an archive.
        Clear clear{this->decrypt(cipher)};

        // Deserialize the object from the clear.
        clear.buffer().reader() >> object;
      }

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(SecretKey const&) const;
      ELLE_OPERATOR_NO_ASSIGNMENT(SecretKey);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(SecretKey);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(elle::Buffer, buffer);
    };
  }
}

# include <cryptography/SecretKey.hxx>

#endif
