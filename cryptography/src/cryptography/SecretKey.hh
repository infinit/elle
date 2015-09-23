#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HH
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HH

# include <utility>

# include <elle/Printable.hh>
# include <elle/attribute.hh>
# include <elle/fwd.hh>
# include <elle/operator.hh>
# include <elle/serialization.hh>
# include <elle/types.hh>
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
#  include <elle/concept/Uniquable.hh>
#  include <elle/serialize/construct.hh>
# endif

ELLE_OPERATOR_RELATIONALS();

# include <cryptography/fwd.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/Cipher.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    /// Represent a secret key for symmetric cryptosystem operations.
    class SecretKey:
      public elle::Printable
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      , public elle::serialize::DynamicFormat<SecretKey>
      , public elle::concept::MakeUniquable<SecretKey>
# endif
    {
      /*---------------.
      | Default Values |
      `---------------*/
    public:
      struct defaults
      {
        static Cipher const cipher = Cipher::aes256;
        static Mode const mode = Mode::cbc;
        static Oneway const oneway = Oneway::sha256;
      };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Construct a secret key by providing the cipher algorithm and key
      /// length, in bits, along with the oneway algorithm used internally.
      SecretKey(elle::String const& password
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
                , Cipher const cipher = defaults::cipher
                , Mode const mode = defaults::mode
                , Oneway const oneway = defaults::oneway
# endif
               );
      /// Construct a secret key based on a given buffer.
      SecretKey(elle::Buffer&& password
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
                , Cipher const cipher = defaults::cipher
                , Mode const mode = defaults::mode
                , Oneway const oneway = defaults::oneway
# endif
               );
      SecretKey(SecretKey const& other);
      SecretKey(SecretKey&& other);
      virtual
      ~SecretKey() = default;

      /*--------.
      | Methods |
      `--------*/
    public:
# if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      /// Encipher a given plain text and return the cipher text.
      elle::Buffer
      encipher(elle::ConstWeakBuffer const& plain,
               Cipher const cipher = defaults::cipher,
               Mode const mode = defaults::mode,
               Oneway const oneway = defaults::oneway) const;
      /// Decipher a given code and return the original plain text.
      elle::Buffer
      decipher(elle::ConstWeakBuffer const& code,
               Cipher const cipher = defaults::cipher,
               Mode const mode = defaults::mode,
               Oneway const oneway = defaults::oneway) const;
# endif
      /// Encipher an input stream and put the cipher text in the
      /// output stream.
      void
      encipher(std::istream& plain,
               std::ostream& code,
               Cipher const cipher = defaults::cipher,
               Mode const mode = defaults::mode,
               Oneway const oneway = defaults::oneway) const;
      /// Decipher an input stream and put the deciphered text in the
      /// output stream.
      void
      decipher(std::istream& code,
               std::ostream& plain,
               Cipher const cipher = defaults::cipher,
               Mode const mode = defaults::mode,
               Oneway const oneway = defaults::oneway) const;
      /// Return the size, in bytes, of the secret key.
      elle::Natural32
      size() const;
      /// Return the length, in bits, of the secret key.
      elle::Natural32
      length() const;

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(SecretKey const&) const;
      SecretKey& operator=(const SecretKey& b) = delete;
      SecretKey& operator=(SecretKey&& b) = default;

      /*----------.
      | Printable |
      `----------*/
    public:
      void
      print(std::ostream& stream) const override;

      /*--------------.
      | Serialization |
      `--------------*/
    public:
      SecretKey(elle::serialization::SerializerIn& serializer);
      void
      serialize(elle::serialization::Serializer& serializer);
      typedef elle::serialization_tag serialization_tag;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(elle::Buffer, password);

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      /*-------.
      | Legacy |
      `-------*/
    public:
      // construction
      SecretKey() {}
      ELLE_SERIALIZE_CONSTRUCT(SecretKey,
                               _password)
      {}
      // methods
      template <typename T = Plain>
      Code
      encrypt(T const& value) const;
      template <typename T = Clear>
      T
      decrypt(Code const& code) const;
      Code
      legacy_encrypt_buffer(elle::Buffer const& buffer) const;
      elle::Buffer
      legacy_decrypt_buffer(Code const& code) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(SecretKey);
      using elle::serialize::SerializableMixin<
        infinit::cryptography::SecretKey,
        elle::serialize::Base64Archive>::serialize;
      // attributes
      ELLE_ATTRIBUTE_R(Cipher, cipher);
      // XXX FIXME: ELLE_ATTRIBUTE_R of Mode doesn't compile on GCC 4.7.
      Mode _mode;
      ELLE_ATTRIBUTE_R(Oneway, oneway);
# endif
    };
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace secretkey
    {
      /*----------.
      | Functions |
      `----------*/

      /// Return a freshly generated secret key of the given length.
      ///
      /// Note that the length is expressed in bits.
      SecretKey
      generate(elle::Natural32 const length
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
               , Cipher const cipher = SecretKey::defaults::cipher
               , Mode const mode = SecretKey::defaults::mode
               , Oneway const oneway = SecretKey::defaults::oneway
# endif
              );
    }
  }
}

# include <cryptography/SecretKey.hxx>

#endif
