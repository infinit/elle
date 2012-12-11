#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HH
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>

# include <cryptography/fwd.hh>

# include <openssl/evp.h>
# include <openssl/err.h>
# include <openssl/rand.h>
# include <fcntl.h>

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
      /// Define the magic salt which is embedded in every encrypted
      /// text so for the decryption process to know that the text
      /// has been salted.
      static elle::Character const magic[];

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
      SecretKey(elle::String const& password);
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
      encrypt(T const& plain) const;
      /// Decrypt the given cipher and return a clear text i.e the
      /// equivalent of the plain text provided as input to the
      /// encryption process.
      Clear
      decrypt(Cipher const& cipher) const;
      /// Decrypt a cipher and return a deserializable object.
      template <typename T>
      T
      decrypt(Cipher const& cipher) const;

      /// XXX
      template<typename T>
        elle::Status Decrypt(Cipher const& in,
                       elle::serialize::Polymorphic<T> const& out) const
        {
          return this->Decrypt<elle::serialize::Polymorphic<T> const>(in, out);
        }

      //
      // interfaces
      //

      // object
      declare(SecretKey);
      elle::Boolean           operator==(const SecretKey&) const;
      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

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
