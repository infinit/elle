#ifndef INFINIT_CRYPTOGRAPHY_CIPHER_HH
# define INFINIT_CRYPTOGRAPHY_CIPHER_HH

# include <elle/serialization/Serializer.hh>

# include <openssl/evp.h>

namespace infinit
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
    struct Serialize<infinit::cryptography::Cipher>
    {
      typedef uint8_t Type;
      static
      uint8_t
      convert(infinit::cryptography::Cipher const& value);
      static
      infinit::cryptography::Cipher
      convert(uint8_t const& representation);
    };

    // Mode
    template <>
    struct Serialize<infinit::cryptography::Mode>
    {
      typedef uint8_t Type;
      static
      uint8_t
      convert(infinit::cryptography::Mode const& value);
      static
      infinit::cryptography::Mode
      convert(uint8_t const& representation);
    };
  }
}

#endif
