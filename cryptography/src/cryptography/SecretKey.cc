#include <cryptography/SecretKey.hh>
#include <cryptography/Digest.hh>
#include <cryptography/random.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/evp.hh>

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

    SecretKey::SecretKey()
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

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
      _password(other._password.contents(), other._password.size()),
      _cipher(other._cipher),
      _mode(other._mode),
      _oneway(other._oneway)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(SecretKey&& other):
      _password(std::move(other._password)),
      _cipher(other._cipher),
      _mode(other._mode),
      _oneway(other._oneway)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(SecretKey,
                                    _password)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    /*--------.
    | Methods |
    `--------*/

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
      elle::fprintf(stream, "%x[%s, %s, %s]",
                    this->_password,
                    this->_cipher, this->_mode, this->_oneway);
    }
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
