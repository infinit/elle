#include <cryptography/SecretKey.hh>
#include <cryptography/Digest.hh>
#include <cryptography/random.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/evp.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");

namespace infinit
{
  namespace cryptography
  {
    /*---------------.
    | Static Methods |
    `---------------*/

    SecretKey
    SecretKey::generate(cipher::Algorithm const cipher,
                        elle::Natural32 const length,
                        oneway::Algorithm const oneway)
    {
      ELLE_TRACE_FUNCTION(cipher, length, oneway);

      // Convert the length in a bit-specific size.
      elle::Natural32 size = length / 8;

      // Generate a buffer-based password.
      elle::Buffer password{random::generate<elle::Buffer>(size)};

      // Return a new secret key.
      return (SecretKey{cipher, std::move(password), oneway});
    }

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
      ELLE_TRACE_METHOD(plain);

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
      ELLE_TRACE_METHOD(code);

      // Resolve the cipher and oneway functions.
      ::EVP_CIPHER const* function_cipher = cipher::resolve(this->_cipher);
      ::EVP_MD const* function_oneway = oneway::resolve(this->_oneway);

      return (evp::symmetric::decrypt(code,
                                      this->_password,
                                      function_cipher,
                                      function_oneway));
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    SecretKey::operator ==(SecretKey const& other) const
    {
      if (this == &other)
        return (true);

      // Compare the internal buffers.
      return (this->_password == other._password);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    SecretKey::print(std::ostream& stream) const
    {
      stream << this->_cipher << "(" << this->_password << ")";
    }
  }
}
