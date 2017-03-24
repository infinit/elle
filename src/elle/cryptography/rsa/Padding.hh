#ifndef ELLE_CRYPTOGRAPHY_RSA_PADDING_HH
# define ELLE_CRYPTOGRAPHY_RSA_PADDING_HH

# include <elle/serialization/Serializer.hh>
# include <elle/cryptography/fwd.hh>
# include <iosfwd>

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      /*-------------.
      | Enumerations |
      `-------------*/

      /// Define the padding.
      enum class Padding
      {
        none,
        pkcs1,
        sslv23,
        oaep,
        x931,
        pss
      };

      /*----------.
      | Operators |
      `----------*/

      std::ostream&
      operator <<(std::ostream& stream,
                  Padding const padding);

      namespace padding
      {
        /*----------.
        | Functions |
        `----------*/

        /// Return the value associated with the given padding.
        int
        resolve(Padding const name);
        /// Return the Padding associated with the given internal OpenSSL value.
        Padding
        resolve(int const value);
        /// Print information regarding the given EVP context's padding.
        void
        print(std::ostream& stream,
              ::EVP_PKEY_CTX* context);
        /// Return the padding's footprint --- i.e the number of bits (not
        /// bytes) of data reserved for the padding --- for the given encryption
        /// context.
        uint32_t
        footprint(::EVP_PKEY_CTX* context);
        /// Pad a context.
        void
        pad(::EVP_PKEY_CTX* context,
            Padding const padding);
      }
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
    template <>
    struct Serialize<elle::cryptography::rsa::Padding>
    {
      using Type = uint8_t;
      static
      uint8_t
      convert(elle::cryptography::rsa::Padding const& value);
      static
      elle::cryptography::rsa::Padding
      convert(uint8_t const& representation);
    };
  }
}

#endif
