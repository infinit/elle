#pragma once

#include <utility>

#include <boost/operators.hpp>

#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/fwd.hh>
#include <elle/operator.hh>
#include <elle/serialization.hh>
#include <elle/cryptography/fwd.hh>
#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/Cipher.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    /// Represent a secret key for symmetric cryptosystem operations.
    class SecretKey
      : public elle::Printable
      , private boost::totally_ordered<SecretKey>
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
      SecretKey(std::string const& password);
      /// Construct a secret key based on a given buffer.
      SecretKey(elle::Buffer&& password);
      SecretKey(SecretKey const& other);
      SecretKey(SecretKey&& other);
      virtual
      ~SecretKey() = default;

      /*--------.
      | Methods |
      `--------*/
    public:
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
      /// Encipher an input stream and put the cipher text in the
      /// output stream.
      virtual
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
      uint32_t
      size() const;
      /// Return the length, in bits, of the secret key.
      uint32_t
      length() const;

      /*----------.
      | Operators |
      `----------*/
    public:
      bool
      operator ==(SecretKey const&) const;
      SecretKey&
      operator =(SecretKey const&) = delete;
      SecretKey&
      operator =(SecretKey&&) = default;

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
      using serialization_tag = elle::serialization_tag;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(elle::Buffer, password);
    };
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

      /// Return a freshly generated secret key of the given length.
      ///
      /// Note that the length is expressed in bits.
      SecretKey
      generate(uint32_t const length);
    }
  }
}
