#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HH
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HH

# include <elle/fwd.hh>
# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/serialize/construct.hh>
# include <elle/concept/Uniquable.hh>

# include <cryptography/fwd.hh>
# include <cryptography/oneway.hh>
# include <cryptography/cipher.hh>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace infinit
{
  namespace cryptography
  {
    /// Represent a secret key for symmetric cryptosystem operations.
    class SecretKey:
      public elle::concept::MakeUniquable<SecretKey>,
      public elle::Printable
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return a freshly generated secret key of the given length.
      ///
      /// Note that the length is expressed in bits.
      static
      SecretKey
      generate(cipher::Algorithm const cipher,
               elle::Natural32 const length,
               oneway::Algorithm const oneway = oneway::Algorithm::sha256);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      SecretKey(); // XXX[to deserialize]
      /// Construct a secret key by providing the cipher algorithm and key
      /// length, in bits, along with the oneway algorithm used internally.
      SecretKey(cipher::Algorithm const cipher,
                elle::String const& password,
                oneway::Algorithm const oneway = oneway::Algorithm::sha256);
      SecretKey(SecretKey const& other);
      SecretKey(SecretKey&& other);
      /// Derialization constructor.
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(SecretKey);
    private:
      /// Construct a secret key based on a given buffer.
      SecretKey(cipher::Algorithm const cipher,
                elle::Buffer&& password,
                oneway::Algorithm const oneway);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Encrypt the given plain text and return a ciphered text i.e
      /// a code.
      Code
      encrypt(Plain const& plain) const;
      /// Encrypt any serializable object and return a ciphered text
      /// of its archive.
      template <typename T>
      Code
      encrypt(T const& value) const;
      /// Decrypt the given code and return a clear text i.e the
      /// equivalent of the plain text provided as input to the
      /// encryption process.
      Clear
      decrypt(Code const& code) const;
      /// Decrypt a code and return a deserializable object.
      template <typename T>
      T
      decrypt(Code const& code) const;

      /// XXX[temporary until the nucleus node factory is set to handle this
      //      case]
      template <typename T>
      void
      decrypt(Code const& code,
              T& object) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
        ELLE_TRACE_METHOD(code);

        Clear clear{this->decrypt(code)};

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
      ELLE_ATTRIBUTE(cipher::Algorithm, cipher);
      ELLE_ATTRIBUTE(elle::Buffer, password);
      ELLE_ATTRIBUTE(oneway::Algorithm, oneway);
    };
  }
}

# include <cryptography/SecretKey.hxx>

#endif
