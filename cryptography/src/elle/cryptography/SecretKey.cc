#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/random.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/cryptography.hh>
#include <elle/cryptography/raw.hh>

#include <elle/serialization/Serializer.hh>
#include <elle/log.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    SecretKey::SecretKey(std::string const& password)
      : _password(password)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(elle::Buffer&& password)
      : _password(std::move(password))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(SecretKey const& other)
      : _password(other._password)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    SecretKey::SecretKey(SecretKey&& other)
      : _password(std::move(other._password))
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
      auto in = elle::IOStream(plain.istreambuf());
      auto res = elle::Buffer();
      res.capacity(plain.size() + 1024);
      {
        auto out = elle::IOStream(res.ostreambuf());
        this->encipher(in, out, cipher, mode, oneway);
      }
      return res;
    }

    elle::Buffer
    SecretKey::decipher(elle::ConstWeakBuffer const& code,
                        Cipher const cipher,
                        Mode const mode,
                        Oneway const oneway) const
    {
      auto in = elle::IOStream(code.istreambuf());
      auto res = elle::Buffer();
      res.capacity(code.size());
      {
        auto out = elle::IOStream(res.ostreambuf());
        this->decipher(in, out, cipher, mode, oneway);
      }
      return res;
    }

    void
    SecretKey::encipher(std::istream& plain,
                        std::ostream& code,
                        Cipher const cipher,
                        Mode const mode,
                        Oneway const oneway) const
    {
      raw::symmetric::encipher(this->_password,
                               cipher::resolve(cipher, mode),
                               oneway::resolve(oneway),
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
      raw::symmetric::decipher(this->_password,
                               cipher::resolve(cipher, mode),
                               oneway::resolve(oneway),
                               code,
                               plain);
    }

    uint32_t
    SecretKey::size() const
    {
      return this->_password.size();
    }

    uint32_t
    SecretKey::length() const
    {
      return this->_password.size() * 8;
    }

    /*----------.
    | Operators |
    `----------*/

    bool
    SecretKey::operator ==(SecretKey const& other) const
    {
      return this->_password == other._password;
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

namespace elle
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
        return random::generate<elle::Buffer>(length / 8);
      }
    }
  }
}
