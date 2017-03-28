#pragma once

#include <elle/cryptography/fwd.hh>
#include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace cryptography
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Define the cipher algorithm.
    enum class Cipher
    {
      null,
      des,
      des2,
      des3,
      desx,
      idea,
      rc2,
      blowfish,
      cast5,
      aes128,
      aes192,
      aes256
    };

    // The mode of operation
    enum class Mode
    {
      none,
      cbc,
      ecb,
      cfb,
      ofb
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Cipher const cipher);

    std::ostream&
    operator <<(std::ostream& stream,
                Mode const mode);

    namespace cipher
    {
      /*----------.
      | Functions |
      `----------*/

      /// Return the cipher function associated with the given algorithm name.
      ::EVP_CIPHER const*
      resolve(Cipher const cipher,
              Mode const mode);
      /// Return the cipher and mode based on the function.
      std::pair<Cipher, Mode>
      resolve(::EVP_CIPHER const* function);
    }
  }
}

/*--------------.
| Serialization |
`--------------*/

namespace elle
{
  namespace serialization
  {
    // Cipher
    template <>
    struct Serialize<elle::cryptography::Cipher>
    {
      using Type = uint8_t;
      static
      uint8_t
      convert(elle::cryptography::Cipher const& value);
      static
      elle::cryptography::Cipher
      convert(uint8_t const& representation);
    };

    // Mode
    template <>
    struct Serialize<elle::cryptography::Mode>
    {
      using Type = uint8_t;
      static
      uint8_t
      convert(elle::cryptography::Mode const& value);
      static
      elle::cryptography::Mode
      convert(uint8_t const& representation);
    };
  }
}
