#include <cryptography/SecretKey.hh>
#include <cryptography/Digest.hh>
#include <cryptography/random.hh>
#include <cryptography/cipher.hh>
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

    SecretKey::SecretKey(cipher::Algorithm const cipher,
                         elle::String const& password,
                         oneway::Algorithm const oneway):
      _cipher(cipher),
      _password(reinterpret_cast<elle::Byte const*>(password.c_str()),
                password.length()),
      _oneway(oneway)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(cipher::Algorithm const cipher,
                         elle::Buffer&& password,
                         oneway::Algorithm const oneway):
      _cipher(cipher),
      _password(std::move(password)),
      _oneway(oneway)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(SecretKey const& other):
      _cipher(other._cipher),
      _password(other._password.contents(), other._password.size()),
      _oneway(other._oneway)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(SecretKey&& other):
      _cipher(other._cipher),
      _password(std::move(other._password)),
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

    Code
    SecretKey::encrypt(Plain const& plain) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("plain: %x", plain);

      // Resolve the cipher and oneway functions.
      ::EVP_CIPHER const* function_cipher = cipher::resolve(this->_cipher);
      ::EVP_MD const* function_oneway = oneway::resolve(this->_oneway);

      return (evp::symmetric::encrypt(plain,
                                      this->_password,
                                      function_cipher,
                                      function_oneway));
    }

    Clear
    SecretKey::decrypt(Code const& code) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("code: %x", code);

      // Resolve the cipher and oneway functions.
      ::EVP_CIPHER const* function_cipher = cipher::resolve(this->_cipher);
      ::EVP_MD const* function_oneway = oneway::resolve(this->_oneway);

      return (evp::symmetric::decrypt(code,
                                      this->_password,
                                      function_cipher,
                                      function_oneway));
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

      // Compare the internal buffer along with the algorithms.
      return ((this->_cipher == other._cipher) &&
              (this->_password == other._password) &&
              (this->_oneway == other._oneway));
    }

    /*----------.
    | Printable |
    `----------*/

    void
    SecretKey::print(std::ostream& stream) const
    {
      elle::fprintf(stream, "%s(%x)", this->_cipher, this->_password);
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
      generate(cipher::Algorithm const cipher,
               elle::Natural32 const length,
               oneway::Algorithm const oneway)
      {
        ELLE_TRACE_FUNCTION(cipher, length, oneway);

        // Convert the length in a bit-specific size.
        elle::Natural32 size = length / 8;

        // Generate a buffer-based password.
        elle::Buffer password(random::generate<elle::Buffer>(size));

        // Return a new secret key.
        return (SecretKey(cipher, std::move(password), oneway));
      }
    }
  }
}
