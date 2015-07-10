#include <cryptography/SecretKey.hh>
#include <cryptography/random.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/raw.hh>

#include <elle/serialization/Serializer.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    SecretKey::SecretKey(elle::String const& password,
                         Cipher const cipher,
                         Mode const mode,
                         Oneway const oneway):
      _password(reinterpret_cast<elle::Byte const*>(password.c_str()),
                password.length()),
      _cipher(cipher),
      _mode(mode),
      _oneway(oneway)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(elle::Buffer&& password,
                         Cipher const cipher,
                         Mode const mode,
                         Oneway const oneway):
      _password(std::move(password)),
      _cipher(cipher),
      _mode(mode),
      _oneway(oneway)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(SecretKey const& other):
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        elle::serialize::DynamicFormat<SecretKey>(other),
#endif
      _password(other._password.contents(), other._password.size()),
      _cipher(other._cipher),
      _mode(other._mode),
      _oneway(other._oneway)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(SecretKey&& other):
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        elle::serialize::DynamicFormat<SecretKey>(other),
#endif
      _password(std::move(other._password)),
      _cipher(other._cipher),
      _mode(other._mode),
      _oneway(other._oneway)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    /*--------.
    | Methods |
    `--------*/

#if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
    elle::Buffer
    SecretKey::encipher(elle::ConstWeakBuffer const& plain) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("plain: %x", plain);

      elle::IOStream _plain(plain.istreambuf());
      std::stringstream _code;

      this->encipher(_plain, _code);

      elle::Buffer code(_code.str().data(), _code.str().length());

      return (code);
    }

    elle::Buffer
    SecretKey::decipher(elle::ConstWeakBuffer const& code) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("code: %x", code);

      elle::IOStream _code(code.istreambuf());
      std::stringstream _plain;

      this->decipher(_code, _plain);

      elle::Buffer plain(_plain.str().data(), _plain.str().length());

      return (plain);
    }
#endif

    void
    SecretKey::encipher(std::istream& plain,
                        std::ostream& code) const
    {
      ELLE_TRACE_METHOD("");

      ::EVP_CIPHER const* function_cipher =
          cipher::resolve(this->_cipher, this->_mode);
      ::EVP_MD const* function_oneway =
          oneway::resolve(this->_oneway);

      raw::symmetric::encipher(plain,
                               code,
                               this->_password,
                               function_cipher,
                               function_oneway);
    }

    void
    SecretKey::decipher(std::istream& code,
                        std::ostream& plain) const
    {
      ELLE_TRACE_METHOD("");

      ::EVP_CIPHER const* function_cipher =
          cipher::resolve(this->_cipher, this->_mode);
      ::EVP_MD const* function_oneway =
          oneway::resolve(this->_oneway);

      raw::symmetric::decipher(code,
                               plain,
                               this->_password,
                               function_cipher,
                               function_oneway);
    }

    elle::Natural32
    SecretKey::size() const
    {
      return (this->_password.size());
    }

    elle::Natural32
    SecretKey::length() const
    {
      return (this->_password.size() * 8);
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    SecretKey::operator ==(SecretKey const& other) const
    {
      if (this == &other)
        return (true);

      // Compare the internal buffer along with the oneways.
      return ((this->_password == other._password) &&
              (this->_cipher == other._cipher) &&
              (this->_mode == other._mode) &&
              (this->_oneway == other._oneway));
    }

    /*----------.
    | Printable |
    `----------*/

    void
    SecretKey::print(std::ostream& stream) const
    {
      elle::fprintf(stream, "%x(%s)[%s, %s, %s]",
                    this->_password,
                    this->_password.size(),
                    this->_cipher, this->_mode, this->_oneway);
    }

    /*--------------.
    | Serialization |
    `--------------*/

    SecretKey::SecretKey(elle::serialization::SerializerIn& serializer)
    {
      this->serialize(serializer);
    }

    void
    SecretKey::serialize(elle::serialization::Serializer& serializer)
    {
      serializer.serialize("password", this->_password);
      serializer.serialize("cipher", this->_cipher);
      serializer.serialize("mode", this->_mode);
      serializer.serialize("oneway", this->_oneway);
    }

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
    /*-------.
    | Legacy |
    `-------*/

    Code
    SecretKey::legacy_encrypt_buffer(elle::Buffer const& plain) const
    {
      ELLE_DEBUG_METHOD("");
      ELLE_DUMP("plain: %x", plain);

      elle::Buffer buffer;
      buffer.writer() << plain;

      return (this->encrypt(Plain(buffer)));
    }

    elle::Buffer
    SecretKey::legacy_decrypt_buffer(Code const& code) const
    {
      ELLE_DEBUG_METHOD("");
      ELLE_DUMP("code: %x", code);

      Clear clear(this->decrypt(code));

      elle::Buffer value;
      clear.buffer().reader() >> value;

      return (value);
    }
#endif
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

      SecretKey
      generate(elle::Natural32 const length,
               Cipher const cipher,
               Mode const mode,
               Oneway const oneway)
      {
        ELLE_TRACE_FUNCTION(length, cipher, mode, oneway);

        // Convert the length in a bit-specific size.
        elle::Natural32 size = length / 8;

        // Generate a buffer-based password.
        elle::Buffer password(random::generate<elle::Buffer>(size));

        // Return a new secret key.
        return (SecretKey(std::move(password), cipher, mode, oneway));
      }
    }
  }
}
