#include <cryptography/SecretKey.hh>
#include <cryptography/random.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/raw.hh>

#include <elle/serialization/Serializer.hh>
#include <elle/log.hh>

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

    SecretKey::SecretKey(std::string const& password):
      _password(reinterpret_cast<uint8_t const*>(password.c_str()),
                password.length())
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(elle::Buffer&& password):
      _password(std::move(password))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(SecretKey const& other):
      _password(other._password.contents(), other._password.size())
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(SecretKey&& other):
      _password(std::move(other._password))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    /*--------.
    | Methods |
    `--------*/

    elle::Buffer
    SecretKey::encipher(elle::ConstWeakBuffer const& plain,
                        Cipher const cipher,
                        Mode const mode,
                        Oneway const oneway) const
    {
      elle::IOStream _plain(plain.istreambuf());
      std::stringstream _code;

      this->encipher(_plain, _code,
                     cipher, mode, oneway);

      elle::Buffer code(_code.str().data(), _code.str().length());

      return (code);
    }

    elle::Buffer
    SecretKey::decipher(elle::ConstWeakBuffer const& code,
                        Cipher const cipher,
                        Mode const mode,
                        Oneway const oneway) const
    {
      elle::IOStream _code(code.istreambuf());
      std::stringstream _plain;

      this->decipher(_code, _plain,
                     cipher, mode, oneway);

      elle::Buffer plain(_plain.str().data(), _plain.str().length());

      return (plain);
    }

    void
    SecretKey::encipher(std::istream& plain,
                        std::ostream& code,
                        Cipher const cipher,
                        Mode const mode,
                        Oneway const oneway) const
    {
      ::EVP_CIPHER const* function_cipher = cipher::resolve(cipher, mode);
      ::EVP_MD const* function_oneway = oneway::resolve(oneway);

      raw::symmetric::encipher(this->_password,
                               function_cipher,
                               function_oneway,
                               plain,
                               code);
    }

    void
    SecretKey::decipher(std::istream& code,
                        std::ostream& plain,
                        Cipher const cipher,
                        Mode const mode,
                        Oneway const oneway) const
    {
      ::EVP_CIPHER const* function_cipher = cipher::resolve(cipher, mode);
      ::EVP_MD const* function_oneway = oneway::resolve(oneway);

      raw::symmetric::decipher(this->_password,
                               function_cipher,
                               function_oneway,
                               code,
                               plain);
    }

    uint32_t
    SecretKey::size() const
    {
      return (this->_password.size());
    }

    uint32_t
    SecretKey::length() const
    {
      return (this->_password.size() * 8);
    }

    /*----------.
    | Operators |
    `----------*/

    bool
    SecretKey::operator ==(SecretKey const& other) const
    {
      return (this->_password == other._password);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    SecretKey::print(std::ostream& stream) const
    {
      elle::fprintf(stream, "%x[%s]",
                    this->_password,
                    this->_password.size());
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
      generate(uint32_t const length)
      {
        // Convert the length in a bit-specific size.
        uint32_t size = length / 8;

        // Generate a buffer-based password.
        elle::Buffer password(random::generate<elle::Buffer>(size));

        // Return a new secret key.
        return (SecretKey(std::move(password)));
      }
    }
  }
}
