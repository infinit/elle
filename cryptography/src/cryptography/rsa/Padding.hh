#ifndef INFINIT_CRYPTOGRAPHY_RSA_PADDING_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PADDING_HH

# include <elle/types.hh>
# include <elle/serialization/Serializer.hh>

# include <openssl/evp.h>

# include <iosfwd>

namespace infinit
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
        elle::Natural32
        footprint(::EVP_PKEY_CTX* context);
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
    struct Serialize<infinit::cryptography::rsa::Padding>
    {
      typedef elle::Natural8 Type;
      static
      elle::Natural8
      convert(infinit::cryptography::rsa::Padding const& value);
      static
      infinit::cryptography::rsa::Padding
      convert(elle::Natural8 const& representation);
    };
  }
}

#endif
